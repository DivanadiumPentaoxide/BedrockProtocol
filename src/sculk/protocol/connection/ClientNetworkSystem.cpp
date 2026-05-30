// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/ClientNetworkSystem.hpp"
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <thread>

namespace sculk::protocol::inline abi_v975 {

namespace {

constexpr auto         RECEIVE_IDLE_SLEEP         = std::chrono::milliseconds(1);
constexpr auto         SEND_FLUSH_INTERVAL        = std::chrono::milliseconds(20);
constexpr std::uint8_t MINECRAFT_BATCH_PACKET_ID  = 0xFE;
constexpr std::size_t  MAX_POOLED_PACKET_CAPACITY = 1U << 20;

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

        buffer.clear();
        (void)mPool.enqueue(std::move(buffer));
    }

private:
    moodycamel::ConcurrentQueue<PacketBuffer> mPool{};
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
  mThreadPool(workerThreadCount),
  mScheduler(mThreadPool) {}

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
        mSendWakeRequested.store(true, std::memory_order_release);
        mIoWaitCv.notify_all();
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

    mIoThread = std::jthread([this](std::stop_token token) { ioLoop(token); });
    return true;
}

void ClientNetworkSystem::disconnect() {
    if (!mRunning.exchange(false, std::memory_order_acq_rel)) {
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

    if (mIoThread.joinable()) {
        mIoThread.request_stop();
        mSendWakeRequested.store(true, std::memory_order_release);
        mIoWaitCv.notify_all();
        mIoThread.join();
    }

    mSession.store(SessionPtr{}, std::memory_order_release);

    if (hasRemote) {
        emitEvent(NetworkEvent{NetworkEventType::Disconnected, remote.rakNetGuid, remote.systemAddress});
    }

    if (mPeer) {
        // Give RakNet a short window to flush disconnection notification.
        mPeer->Shutdown(200, 0, HIGH_PRIORITY);
    }
}

bool ClientNetworkSystem::isConnected() const noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    return session && session->isConnected();
}

bool ClientNetworkSystem::sendPacket(std::span<const std::byte> buffer) noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return false;
    }

    if (!session->sendPacket(buffer)) {
        return false;
    }

    bool expectedWake = false;
    if (mSendWakeRequested.compare_exchange_strong(expectedWake, true, std::memory_order_acq_rel)) {
        mIoWaitCv.notify_one();
    }
    return true;
}

std::uint32_t ClientNetworkSystem::sendPacketImmediately(std::span<const std::byte> buffer) noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session || buffer.empty()) {
        return 0;
    }

    thread_local PacketBufferBatch singlePacketBatch;
    singlePacketBatch.clear();
    singlePacketBatch.emplace_back(buffer.begin(), buffer.end());

    auto batched = Session::serializeBatchedPackets(singlePacketBatch);
    if (batched.empty()) {
        return 0;
    }

    auto receipt = mNextImmediateReceipt.fetch_add(1, std::memory_order_relaxed);
    if (receipt == 0) {
        receipt = mNextImmediateReceipt.fetch_add(1, std::memory_order_relaxed);
    }

    auto framed = prependMinecraftBatchHeader(batched);
    gPacketBufferPool.release(std::move(batched));
    if (!mImmediateSends.enqueue(ImmediateSendRequest{session, std::move(framed), receipt})) {
        gPacketBufferPool.release(std::move(framed));
        return 0;
    }

    bool expectedWake = false;
    if (mSendWakeRequested.compare_exchange_strong(expectedWake, true, std::memory_order_acq_rel)) {
        mIoWaitCv.notify_one();
    }

    return receipt;
}

bool ClientNetworkSystem::receivePacket(std::vector<std::byte>& outBuffer) noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return false;
    }
    return session->receivePacket(outBuffer);
}

bool ClientNetworkSystem::getNetworkStatus(NetworkStatus& outStatus) const noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return false;
    }

    outStatus = session->getNetworkStatus();
    return true;
}

std::uint64_t ClientNetworkSystem::subscribeEvents(std::function<void(const NetworkEvent&)> handler) {
    if (!handler) {
        return 0;
    }

    const std::uint64_t id            = mNextSubscriptionId.fetch_add(1, std::memory_order_relaxed);
    auto                stableHandler = std::move(handler);
    for (;;) {
        auto current = mEventHandlersSnapshot.load(std::memory_order_acquire);
        auto next    = std::make_shared<EventHandlerVec>(*current);
        next->emplace_back(id, stableHandler);

        auto desired = std::const_pointer_cast<const EventHandlerVec>(next);
        if (mEventHandlersSnapshot
                .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
            return id;
        }
    }
}

bool ClientNetworkSystem::unsubscribeEvents(std::uint64_t subscriptionId) {
    if (subscriptionId == 0) {
        return false;
    }

    for (;;) {
        auto current = mEventHandlersSnapshot.load(std::memory_order_acquire);
        auto next    = std::make_shared<EventHandlerVec>();
        next->reserve(current->size());

        bool removed = false;
        for (const auto& entry : *current) {
            if (entry.first == subscriptionId) {
                removed = true;
                continue;
            }
            next->push_back(entry);
        }

        if (!removed) {
            return false;
        }

        auto desired = std::const_pointer_cast<const EventHandlerVec>(next);
        if (mEventHandlersSnapshot
                .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
            return true;
        }
    }
}

void ClientNetworkSystem::ioLoop(std::stop_token stopToken) {
    auto lastFlush = std::chrono::steady_clock::now() - SEND_FLUSH_INTERVAL;

    while (!stopToken.stop_requested() && mRunning.load(std::memory_order_acquire)) {
        bool progressed = false;

        for (RakNet::Packet* packet = mPeer->Receive(); packet != nullptr; packet = mPeer->Receive()) {
            progressed = true;
            processIncomingPacket(packet);
            mPeer->DeallocatePacket(packet);
        }

        const auto now = std::chrono::steady_clock::now();
        if (now - lastFlush >= SEND_FLUSH_INTERVAL) {
            flushOutboundPackets();
            lastFlush = now;
        }

        if (!progressed) {
            const auto nowIdle    = std::chrono::steady_clock::now();
            const auto untilFlush = (lastFlush + SEND_FLUSH_INTERVAL <= nowIdle)
                                      ? std::chrono::steady_clock::duration::zero()
                                      : (lastFlush + SEND_FLUSH_INTERVAL - nowIdle);
            const auto receiveBudget =
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(RECEIVE_IDLE_SLEEP);
            const auto waitBudget = std::min(untilFlush, receiveBudget);

            std::unique_lock lock(mIoWaitMutex);
            (void)mIoWaitCv.wait_for(lock, waitBudget, [this, &stopToken] {
                if (stopToken.stop_requested()) {
                    return true;
                }

                return mSendWakeRequested.exchange(false, std::memory_order_acq_rel);
            });
        }
    }
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

        mRunning.store(false, std::memory_order_release);
        mSendWakeRequested.store(true, std::memory_order_release);
        mIoWaitCv.notify_all();

        if (mPeer) {
            mPeer->Shutdown(0, 0, HIGH_PRIORITY);
        }

        emitEvent(NetworkEvent{eventType, packet->guid, packet->systemAddress});
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
    auto        packets      = Session::deserializeBatchPackets(std::span<const std::byte>{payloadBegin, payloadSize});
    for (auto& payload : packets) {
        (void)session->enqueueInboundPacket(std::move(payload));
    }
}

void ClientNetworkSystem::emitEvent(NetworkEvent event) {
    auto handlers = mEventHandlersSnapshot.load(std::memory_order_acquire);

    for (const auto& [_, handler] : *handlers) {
        auto copiedHandler = handler;
        if (!mThreadPool.submit([event, copiedHandler = std::move(copiedHandler)]() mutable {
                copiedHandler(event);
            })) {
            copiedHandler(event);
        }
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
            (void)immediate.mSession->sendPacketImmediately(immediate.mPayload, immediate.mForceReceiptNumber);
        }
        gPacketBufferPool.release(std::move(immediate.mPayload));
    }

    thread_local OutboundBatch outboundBatch;
    outboundBatch.clear();
    if (session->tryDequeueAllOutboundPackets(outboundBatch) == 0) {
        return;
    }

    thread_local PacketBufferBatch payloadBatch;
    payloadBatch.clear();
    payloadBatch.reserve(outboundBatch.size());

    for (auto& outbound : outboundBatch) {
        payloadBatch.push_back(std::move(outbound.mPayload));
    }

    if (!payloadBatch.empty()) {
        auto batched = Session::serializeBatchedPackets(payloadBatch);
        if (!batched.empty()) {
            auto framed = prependMinecraftBatchHeader(batched);
            (void)session->sendPacketImmediately(framed);
            gPacketBufferPool.release(std::move(batched));
            gPacketBufferPool.release(std::move(framed));
        }
    }
}

void ClientNetworkSystem::RakPeerDeleter::operator()(RakNet::RakPeerInterface* peer) const noexcept {
    if (peer) {
        RakNet::RakPeerInterface::DestroyInstance(peer);
    }
}

} // namespace sculk::protocol::inline abi_v975
