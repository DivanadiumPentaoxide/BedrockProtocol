// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/ClientNetworkSystem.hpp"
#include "sculk/protocol/connection/coro/PumpAdapters.hpp"
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <thread>

namespace sculk::protocol::inline abi_v975 {

namespace {

constexpr auto         RECEIVE_IDLE_SLEEP          = std::chrono::milliseconds(1);
constexpr auto         SEND_FLUSH_INTERVAL         = std::chrono::milliseconds(20);
constexpr std::uint8_t MINECRAFT_BATCH_PACKET_ID   = 0xFE;
constexpr std::size_t  MAX_POOLED_PACKET_CAPACITY  = 1U << 20;
constexpr std::size_t  MAX_POOLED_PACKET_COUNT     = 64;
constexpr std::size_t  MAX_RETAINED_BATCH_CAPACITY = 256;
constexpr std::size_t  MAX_IMMEDIATE_SEND_QUEUE    = 2048;

template <typename T>
void trimVectorCapacity(std::vector<T>& vec, std::size_t maxCapacity) {
    if (vec.capacity() <= maxCapacity) {
        return;
    }

    std::vector<T> released{};
    vec.swap(released);
}

class PacketBufferPool final {
public:
    [[nodiscard]] PacketBuffer acquire(std::size_t minCapacity = 0) {
        PacketBuffer buffer;
        if (!mPool.try_dequeue(buffer)) {
            if (minCapacity > 0) {
                buffer.reserve(minCapacity);
            }
            return buffer;
        }

        mPooledCount.fetch_sub(1, std::memory_order_relaxed);
        buffer.clear();
        if (buffer.capacity() < minCapacity) {
            buffer.reserve(minCapacity);
        }
        return buffer;
    }

    void release(PacketBuffer&& buffer) {
        if (buffer.capacity() > MAX_POOLED_PACKET_CAPACITY) {
            return;
        }

        auto currentCount = mPooledCount.load(std::memory_order_relaxed);
        while (currentCount < MAX_POOLED_PACKET_COUNT) {
            if (mPooledCount.compare_exchange_weak(
                    currentCount,
                    currentCount + 1,
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed
                )) {
                buffer.clear();
                if (mPool.enqueue(std::move(buffer))) {
                    return;
                }

                mPooledCount.fetch_sub(1, std::memory_order_relaxed);
                return;
            }
        }

        buffer.clear();
    }

private:
    moodycamel::ConcurrentQueue<PacketBuffer> mPool{};
    std::atomic<std::size_t>                  mPooledCount{0};
};

PacketBufferPool gPacketBufferPool{};

[[nodiscard]] PacketBuffer prependMinecraftBatchHeader(std::span<const std::byte> payload) {
    PacketBuffer framed = gPacketBufferPool.acquire(payload.size() + 1);
    framed.push_back(static_cast<std::byte>(MINECRAFT_BATCH_PACKET_ID));
    framed.insert(framed.end(), payload.begin(), payload.end());
    return framed;
}

} // namespace

ClientNetworkSystem::ClientNetworkSystem(std::size_t workerThreadCount)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(std::make_unique<thread::ThreadPool>(workerThreadCount)),
  mThreadPool(mOwnedThreadPool.get()),
  mIoRuntime(nullptr),
  mScheduler(*mThreadPool) {}

ClientNetworkSystem::ClientNetworkSystem(thread::ThreadPool& threadPool)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(nullptr),
  mThreadPool(&threadPool),
  mIoRuntime(nullptr),
  mScheduler(*mThreadPool) {}

ClientNetworkSystem::ClientNetworkSystem(io::ClientIoRuntime& ioRuntime, std::size_t workerThreadCount)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(std::make_unique<thread::ThreadPool>(workerThreadCount)),
  mThreadPool(mOwnedThreadPool.get()),
  mIoRuntime(&ioRuntime),
  mScheduler(*mThreadPool) {
    mUsesSharedIoRuntime = true;
}

ClientNetworkSystem::ClientNetworkSystem(thread::ThreadPool& threadPool, io::ClientIoRuntime& ioRuntime)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(nullptr),
  mThreadPool(&threadPool),
  mIoRuntime(&ioRuntime),
  mScheduler(*mThreadPool) {
    mUsesSharedIoRuntime = true;
}

ClientNetworkSystem::~ClientNetworkSystem() { disconnect(); }

bool ClientNetworkSystem::connect(
    const std::string& host,
    std::uint16_t      remotePort,
    std::uint16_t      localPort,
    std::uint32_t      connectionAttemptCount,
    std::uint16_t      socketFamily
) {
    if (mRunning.load(std::memory_order_acquire)) {
        return true;
    }

    if (mIoThread.joinable()) {
        mIoThread.request_stop();
        mIoWakeSignal.release();
        mIoThread.join();
    }

    bool expectedRunning = false;
    if (!mRunning.compare_exchange_strong(expectedRunning, true, std::memory_order_acq_rel)) {
        return true;
    }

    RakNet::SocketDescriptor socketDescriptor{localPort, nullptr};
    socketDescriptor.socketFamily = socketFamily;
    const auto startupResult      = mPeer->Startup(1, &socketDescriptor, 1);
    if (startupResult != RakNet::RAKNET_STARTED) {
        mRunning.store(false, std::memory_order_release);
        emitEvent(NetworkEvent{NetworkEventType::ConnectionFailed});
        return false;
    }

    const auto connectResult = mPeer->Connect(host.c_str(), remotePort, nullptr, 0, nullptr, 0, connectionAttemptCount);

    if (connectResult != RakNet::CONNECTION_ATTEMPT_STARTED) {
        mPeer->Shutdown(0);
        mRunning.store(false, std::memory_order_release);
        emitEvent(NetworkEvent{NetworkEventType::ConnectionFailed});
        return false;
    }

    mLastFlushTime = std::chrono::steady_clock::now() - SEND_FLUSH_INTERVAL;
    if (mUsesSharedIoRuntime) {
        mRegisteredInSharedIoRuntime.store(true, std::memory_order_release);
        mIoRuntime->registerClient(*this);
        mIoRuntime->notifyWork();
    } else {
        mIoThread = std::jthread([this](std::stop_token token) { ioLoop(token); });
    }
    return true;
}

void ClientNetworkSystem::disconnect() {
    const bool wasRunning                  = mRunning.exchange(false, std::memory_order_acq_rel);
    bool       needUnregisterSharedRuntime = false;
    if (mUsesSharedIoRuntime) {
        needUnregisterSharedRuntime = mRegisteredInSharedIoRuntime.exchange(false, std::memory_order_acq_rel);
    }

    if (!wasRunning && !needUnregisterSharedRuntime) {
        return;
    }

    RakNet::AddressOrGUID remote{};
    bool                  hasRemote = false;

    auto session = mSession.load(std::memory_order_acquire);
    if (session) {
        remote = session->remoteEndpoint();
        session->markDisconnected();
        hasRemote = !remote.IsUndefined();
    }

    // Graceful disconnect: actively notify remote before shutting down local peer.
    if (hasRemote && mPeer) {
        mPeer->CloseConnection(remote, true, 0, HIGH_PRIORITY);
    }

    if (needUnregisterSharedRuntime) {
        mIoRuntime->unregisterClient(*this);
    } else if (mIoThread.joinable()) {
        mIoThread.request_stop();
        mIoWakeSignal.release();
        mIoThread.join();
    }

    mSession.store(SessionPtr{}, std::memory_order_release);
    mPacketPumpActive.store(false, std::memory_order_release);

    if (hasRemote) {
        emitEvent(NetworkEvent{NetworkEventType::Disconnected, remote.rakNetGuid, remote.systemAddress});
    }

    // Eagerly release all event hook references to break potential ownership cycles.
    mOnConnectedHandler.store({}, std::memory_order_release);
    mOnDisconnectedHandler.store({}, std::memory_order_release);
    mOnConnectionFailedHandler.store({}, std::memory_order_release);
    mOnPacketReceiveHandler.store({}, std::memory_order_release);

    if (mPeer) {
        // Give RakNet a short window to flush disconnection notification.
        mPeer->Shutdown(200, 0, HIGH_PRIORITY);
    }
}

bool ClientNetworkSystem::isConnected() const noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    return session && session->isConnected();
}

bool ClientNetworkSystem::sendBuffer(std::span<const std::byte> buffer) {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return false;
    }

    if (!session->sendPacketBuffer(buffer)) {
        return false;
    }

    notifyIoWorker();
    return true;
}

std::uint32_t ClientNetworkSystem::sendBufferImmediately(std::span<const std::byte> buffer) {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session || buffer.empty()) {
        return 0;
    }

    if (mImmediateSends.size_approx() >= MAX_IMMEDIATE_SEND_QUEUE) {
        return 0;
    }

    auto receipt = mNextImmediateReceipt.fetch_add(1, std::memory_order_relaxed);
    if (receipt == 0) {
        receipt = mNextImmediateReceipt.fetch_add(1, std::memory_order_relaxed);
    }

    PacketBuffer payload(buffer.begin(), buffer.end());
    if (!mImmediateSends.enqueue(ImmediateSendRequest{session, std::move(payload), receipt})) {
        return 0;
    }

    notifyIoWorker();

    return receipt;
}

bool ClientNetworkSystem::receiveBuffer(std::vector<std::byte>& outBuffer) noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return false;
    }
    return session->receivePacketBuffer(outBuffer);
}

coro::Task<Result<std::vector<std::byte>>> ClientNetworkSystem::receiveBufferAsync() {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        co_return error_utils::makeError("no active session");
    }

    co_return co_await session->receivePacketBufferAsync();
}

bool ClientNetworkSystem::getNetworkStatus(NetworkStatus& outStatus) const noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return false;
    }

    outStatus = session->getNetworkStatus();
    return true;
}

bool ClientNetworkSystem::setOnConnected(RawEventCallback callback, void* userData) noexcept {
    return setOnEventRaw(mOnConnectedHandler, callback, userData, nullptr);
}

bool ClientNetworkSystem::setOnDisconnected(RawEventCallback callback, void* userData) noexcept {
    return setOnEventRaw(mOnDisconnectedHandler, callback, userData, nullptr);
}

bool ClientNetworkSystem::setOnConnectionFailed(RawEventCallback callback, void* userData) noexcept {
    return setOnEventRaw(mOnConnectionFailedHandler, callback, userData, nullptr);
}

bool ClientNetworkSystem::setOnPacketReceive(RawPacketReceiveCallback callback, void* userData) {
    return setOnPacketRaw(callback, userData, nullptr);
}

bool ClientNetworkSystem::setOnEventRaw(
    std::atomic<std::shared_ptr<EventHook>>& target,
    RawEventCallback                         callback,
    void*                                    userData,
    void (*destroyUserData)(void*) noexcept
) noexcept {
    if (!callback) {
        if (destroyUserData && userData) {
            destroyUserData(userData);
        }
        target.store(std::shared_ptr<EventHook>{}, std::memory_order_release);
        return false;
    }

    std::shared_ptr<EventHook> hook{new (std::nothrow) EventHook{}};
    if (!hook) {
        if (destroyUserData && userData) {
            destroyUserData(userData);
        }
        return false;
    }
    hook->mCallback        = callback;
    hook->mUserData        = userData;
    hook->mDestroyUserData = destroyUserData;
    target.store(std::move(hook), std::memory_order_release);
    return true;
}

bool ClientNetworkSystem::setOnPacketRaw(
    RawPacketReceiveCallback callback,
    void*                    userData,
    void (*destroyUserData)(void*) noexcept
) {
    if (!callback) {
        if (destroyUserData && userData) {
            destroyUserData(userData);
        }
        mOnPacketReceiveHandler.store(std::shared_ptr<PacketHook>{}, std::memory_order_release);
        auto session = mSession.load(std::memory_order_acquire);
        if (session) {
            session->cancelReceiveWaiters();
        }
        return false;
    }

    std::shared_ptr<PacketHook> hook{new (std::nothrow) PacketHook{}};
    if (!hook) {
        if (destroyUserData && userData) {
            destroyUserData(userData);
        }
        return false;
    }

    hook->mCallback        = callback;
    hook->mUserData        = userData;
    hook->mDestroyUserData = destroyUserData;
    mOnPacketReceiveHandler.store(std::move(hook), std::memory_order_release);

    startPacketPumpIfNeeded();
    return true;
}

void ClientNetworkSystem::startPacketPumpIfNeeded() {
    auto packetHook = mOnPacketReceiveHandler.load(std::memory_order_acquire);
    if (!packetHook || !packetHook->mCallback) {
        return;
    }

    auto session = mSession.load(std::memory_order_acquire);
    if (!session || !session->isConnected()) {
        return;
    }

    bool expected = false;
    if (!mPacketPumpActive.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
        return;
    }

    const auto expectedGeneration = session->receivePumpGeneration();

    coro::startPacketPump(
        mScheduler,
        [session, expectedGeneration]() noexcept -> coro::Task<Result<std::vector<std::byte>>> {
            co_return co_await session->receivePacketBufferAsync(expectedGeneration);
        },
        [this](std::vector<std::byte>&& packet) noexcept -> bool {
            auto currentHook = mOnPacketReceiveHandler.load(std::memory_order_acquire);
            if (!currentHook || !currentHook->mCallback) {
                auto session = mSession.load(std::memory_order_acquire);
                if (session && session->isConnected()) {
                    (void)session->enqueueInboundPacket(std::move(packet));
                }
                return false;
            }

            currentHook->mCallback(currentHook->mUserData, std::move(packet));
            return true;
        },
        [this]() noexcept {
            mPacketPumpActive.store(false, std::memory_order_release);

            auto currentHook = mOnPacketReceiveHandler.load(std::memory_order_acquire);
            if (!currentHook || !currentHook->mCallback) {
                return;
            }

            auto session = mSession.load(std::memory_order_acquire);
            if (!session || !session->isConnected()) {
                return;
            }

            startPacketPumpIfNeeded();
        }
    );
}

std::uint64_t ClientNetworkSystem::droppedEventCallbackCount() const noexcept {
    return mDroppedEventCallbacks.load(std::memory_order_relaxed);
}

void ClientNetworkSystem::ioLoop(std::stop_token stopToken) {
    while (!stopToken.stop_requested() && mRunning.load(std::memory_order_acquire)) {
        mSendWakeRequested.store(false, std::memory_order_release);
        const bool progressed = ioTickOnce();

        if (!progressed) {
            const auto nowIdle    = std::chrono::steady_clock::now();
            const auto untilFlush = (mLastFlushTime + SEND_FLUSH_INTERVAL <= nowIdle)
                                      ? std::chrono::steady_clock::duration::zero()
                                      : (mLastFlushTime + SEND_FLUSH_INTERVAL - nowIdle);
            const auto receiveBudget =
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(RECEIVE_IDLE_SLEEP);
            const auto waitBudget = std::min(untilFlush, receiveBudget);

            if (waitBudget > std::chrono::steady_clock::duration::zero() && !stopToken.stop_requested()) {
                (void)mIoWakeSignal.try_acquire_for(waitBudget);
            }
        }
    }
}

bool ClientNetworkSystem::ioTickOnce() noexcept {
    bool progressed = false;

    for (RakNet::Packet* packet = mPeer->Receive(); packet != nullptr; packet = mPeer->Receive()) {
        progressed = true;
        processIncomingPacket(packet);
        mPeer->DeallocatePacket(packet);
    }

    auto       session = mSession.load(std::memory_order_acquire);
    const bool hasPendingSendWork =
        session && (session->hasPendingOutboundPackets() || mImmediateSends.size_approx() > 0);

    const auto now = std::chrono::steady_clock::now();
    if (hasPendingSendWork || now - mLastFlushTime >= SEND_FLUSH_INTERVAL) {
        flushOutboundPackets();
        mLastFlushTime = now;
        progressed     = true;
    }

    return progressed;
}

void ClientNetworkSystem::notifyIoWorker() noexcept {
    if (mSendWakeRequested.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    if (mUsesSharedIoRuntime) {
        mIoRuntime->notifyWork();
        return;
    }

    mIoWakeSignal.release();
}

void ClientNetworkSystem::processIncomingPacket(RakNet::Packet* packet) {
    if (!packet || !packet->data || packet->length == 0) {
        return;
    }

    const auto messageId = packet->data[0];

    if (messageId == ToMessageID(ID_CONNECTION_REQUEST_ACCEPTED)) {
        auto remote          = RakNet::AddressOrGUID{packet->guid};
        remote.systemAddress = packet->systemAddress;

        auto session = std::make_shared<Session>(mPeer.get(), remote, &mScheduler);
        mSession.store(std::move(session), std::memory_order_release);
        emitEvent(NetworkEvent{NetworkEventType::Connected, packet->guid, packet->systemAddress});
        startPacketPumpIfNeeded();
        return;
    }

    if (messageId == ToMessageID(ID_DISCONNECTION_NOTIFICATION) || messageId == ToMessageID(ID_CONNECTION_LOST)
        || messageId == ToMessageID(ID_CONNECTION_ATTEMPT_FAILED)) {
        const auto eventType = (messageId == ToMessageID(ID_CONNECTION_ATTEMPT_FAILED))
                                 ? NetworkEventType::ConnectionFailed
                                 : NetworkEventType::Disconnected;
        auto       session   = mSession.exchange(SessionPtr{}, std::memory_order_acq_rel);
        if (session) {
            session->markDisconnected();
        }
        mPacketPumpActive.store(false, std::memory_order_release);

        mRunning.store(false, std::memory_order_release);
        notifyIoWorker();

        if (mPeer) {
            mPeer->Shutdown(0, 0, HIGH_PRIORITY);
        }

        emitEvent(NetworkEvent{eventType, packet->guid, packet->systemAddress});

        // Eagerly release all event hook references to break potential ownership cycles.
        mOnConnectedHandler.store({}, std::memory_order_release);
        mOnDisconnectedHandler.store({}, std::memory_order_release);
        mOnConnectionFailedHandler.store({}, std::memory_order_release);
        mOnPacketReceiveHandler.store({}, std::memory_order_release);
        return;
    }

    if (messageId != MINECRAFT_BATCH_PACKET_ID || packet->length <= 1) {
        return;
    }

    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return;
    }

    const auto* payloadBegin = reinterpret_cast<const std::byte*>(packet->data + 1);
    const auto  payloadSize  = static_cast<std::size_t>(packet->length - 1);
    auto        packets      = session->deserializeBatchPackets(std::span<const std::byte>{payloadBegin, payloadSize});
    if (!packets) {
        return;
    }

    for (auto& payload : *packets) {
        (void)session->enqueueInboundPacket(std::move(payload));
    }
}

void ClientNetworkSystem::emitEvent(NetworkEvent event) {
    std::shared_ptr<EventHook> handler;
    switch (event.mType) {
    case NetworkEventType::Connected:
        handler = mOnConnectedHandler.load(std::memory_order_acquire);
        break;
    case NetworkEventType::Disconnected:
        handler = mOnDisconnectedHandler.load(std::memory_order_acquire);
        break;
    case NetworkEventType::ConnectionFailed:
        handler = mOnConnectionFailedHandler.load(std::memory_order_acquire);
        break;
    default:
        return;
    }

    if (!handler || !handler->mCallback) {
        return;
    }

    auto sharedHandler = std::move(handler);
    if (!mThreadPool->submit([event, sharedHandler = std::move(sharedHandler)]() mutable noexcept {
            if (sharedHandler && sharedHandler->mCallback) {
                sharedHandler->mCallback(sharedHandler->mUserData, event);
            }
        })) {
        mDroppedEventCallbacks.fetch_add(1, std::memory_order_relaxed);
    }
}

void ClientNetworkSystem::flushOutboundPackets() {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return;
    }

    ImmediateSendRequest immediate;
    while (mImmediateSends.try_dequeue(immediate)) {
        if (immediate.mSession && immediate.mSession->isConnected()) {
            (void)immediate.mSession->sendPacketBufferImmediately(immediate.mPayload, immediate.mForceReceiptNumber);
        }
    }

    thread_local OutboundBatch outboundBatch;
    outboundBatch.clear();
    if (session->tryDequeueAllOutboundPackets(outboundBatch) == 0) {
        trimVectorCapacity(outboundBatch, MAX_RETAINED_BATCH_CAPACITY);
        return;
    }

    thread_local PacketBufferBatch payloadBatch;
    payloadBatch.clear();
    payloadBatch.reserve(outboundBatch.size());

    for (auto& outbound : outboundBatch) {
        payloadBatch.push_back(std::move(outbound.mPayload));
    }

    if (!payloadBatch.empty()) {
        auto batched = session->serializeBatchedPackets(payloadBatch);
        if (!batched.empty()) {
            auto framed = prependMinecraftBatchHeader(batched);
            (void)session->sendBatchedPacketBufferImmediately(framed);
            gPacketBufferPool.release(std::move(framed));
        }
    }

    trimVectorCapacity(outboundBatch, MAX_RETAINED_BATCH_CAPACITY);
    trimVectorCapacity(payloadBatch, MAX_RETAINED_BATCH_CAPACITY);
}

void ClientNetworkSystem::RakPeerDeleter::operator()(RakNet::RakPeerInterface* peer) const noexcept {
    if (peer) {
        RakNet::RakPeerInterface::DestroyInstance(peer);
    }
}

} // namespace sculk::protocol::inline abi_v975
