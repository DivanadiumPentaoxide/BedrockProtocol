// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/ServerNetworkSystem.hpp"
#include "sculk/protocol/connection/coro/PumpAdapters.hpp"
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <limits>
#include <thread>

namespace sculk::protocol::inline abi_v975 {

namespace {

constexpr auto         RECEIVE_IDLE_SLEEP               = std::chrono::milliseconds(1);
constexpr auto         SEND_FLUSH_INTERVAL              = std::chrono::milliseconds(20);
constexpr std::uint8_t MINECRAFT_BATCH_PACKET_ID        = 0xFE;
constexpr std::size_t  MAX_POOLED_PACKET_CAPACITY       = 1U << 20;
constexpr std::size_t  MAX_POOLED_PACKET_COUNT          = 64;
constexpr std::size_t  MAX_RETAINED_BATCH_CAPACITY      = 256;
constexpr std::size_t  MAX_IMMEDIATE_SEND_QUEUE         = 4096;
constexpr std::size_t  HARD_MAX_SESSIONS_PER_FLUSH_PASS = 2048;
constexpr auto         HARD_MAX_FLUSH_TIME_BUDGET       = std::chrono::milliseconds(8);
constexpr std::size_t  ADAPTIVE_BUDGET_LEVELS           = 4;
constexpr std::size_t  LOW_LOAD_FAST_PATH_MAX_SCHEDULED = 64;
constexpr std::size_t  LOW_LOAD_SESSION_BUDGET          = 32;
constexpr auto         LOW_LOAD_TIME_BUDGET             = std::chrono::microseconds(700);
constexpr std::uint8_t PROMOTE_STREAK_THRESHOLD         = 3;
constexpr std::uint8_t DEMOTE_STREAK_THRESHOLD          = 8;

constexpr std::array<std::size_t, ADAPTIVE_BUDGET_LEVELS> SESSION_BUDGETS_PER_LEVEL{
    16,
    64,
    256,
    1024,
};

constexpr std::array<std::chrono::steady_clock::duration, ADAPTIVE_BUDGET_LEVELS> TIME_BUDGETS_PER_LEVEL{
    std::chrono::microseconds(500),
    std::chrono::microseconds(1500),
    std::chrono::microseconds(4000),
    std::chrono::microseconds(8000),
};

// Promote aggressively on sustained pressure; demote conservatively to avoid oscillation.
constexpr std::array<std::size_t, ADAPTIVE_BUDGET_LEVELS> PROMOTE_AT_SCHEDULED{
    96,
    256,
    2048,
    std::numeric_limits<std::size_t>::max(),
};

constexpr std::array<std::size_t, ADAPTIVE_BUDGET_LEVELS> DEMOTE_AT_SCHEDULED{
    0,
    24,
    128,
    1024,
};

struct PreparedSend {
    PacketBuffer  mPayload{};
    std::uint32_t mForceReceiptNumber{};
};

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

void prepareBatchedSendsForSession(const std::shared_ptr<Session>& session, PreparedSend& outPrepared) {
    outPrepared.mPayload.clear();
    outPrepared.mForceReceiptNumber = 0;
    if (!session) {
        return;
    }

    thread_local OutboundBatch     outboundBatch;
    thread_local PacketBufferBatch payloadBatch;
    outboundBatch.clear();
    payloadBatch.clear();

    if (session->tryDequeueAllOutboundPackets(outboundBatch) == 0) {
        trimVectorCapacity(outboundBatch, MAX_RETAINED_BATCH_CAPACITY);
        trimVectorCapacity(payloadBatch, MAX_RETAINED_BATCH_CAPACITY);
        return;
    }

    payloadBatch.reserve(outboundBatch.size());

    for (auto& outbound : outboundBatch) {
        payloadBatch.push_back(std::move(outbound.mPayload));
    }

    if (!payloadBatch.empty()) {
        auto batched = session->serializeBatchedPackets(payloadBatch);
        if (!batched.empty()) {
            outPrepared.mPayload = std::move(batched);
        }
    }

    trimVectorCapacity(outboundBatch, MAX_RETAINED_BATCH_CAPACITY);
    trimVectorCapacity(payloadBatch, MAX_RETAINED_BATCH_CAPACITY);
}

} // namespace

ServerNetworkSystem::ServerNetworkSystem(std::size_t workerThreadCount)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(std::make_unique<thread::ThreadPool>(workerThreadCount)),
  mThreadPool(mOwnedThreadPool.get()),
  mScheduler(*mThreadPool) {}

ServerNetworkSystem::ServerNetworkSystem(thread::ThreadPool& threadPool)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(nullptr),
  mThreadPool(&threadPool),
  mScheduler(*mThreadPool) {}

ServerNetworkSystem::~ServerNetworkSystem() { stop(); }

bool ServerNetworkSystem::start(std::uint16_t ipv4Port, std::uint32_t maxConnections) {
    if (mRunning.exchange(true, std::memory_order_acq_rel)) {
        return true;
    }

    RakNet::SocketDescriptor socketDescriptor{ipv4Port, nullptr};
    socketDescriptor.socketFamily = AF_INET;

    const auto startupResult = mPeer->Startup(maxConnections, &socketDescriptor, 1);
    if (startupResult != RakNet::RAKNET_STARTED) {
        mRunning.store(false, std::memory_order_release);
        return false;
    }

    mPeer->SetMaximumIncomingConnections(static_cast<std::uint16_t>(maxConnections));
    mIoThread = std::jthread([this](std::stop_token token) { ioLoop(token); });
    return true;
}

bool ServerNetworkSystem::start(std::uint16_t ipv4Port, std::uint16_t ipv6Port, std::uint32_t maxConnections) {
    if (mRunning.exchange(true, std::memory_order_acq_rel)) {
        return true;
    }

    std::array<RakNet::SocketDescriptor, 2> socketDescriptors{
        RakNet::SocketDescriptor{ipv4Port, nullptr},
        RakNet::SocketDescriptor{ipv6Port, nullptr}
    };
    socketDescriptors[0].socketFamily = AF_INET;
    socketDescriptors[1].socketFamily = AF_INET6;

    const auto startupResult =
        mPeer->Startup(maxConnections, socketDescriptors.data(), static_cast<unsigned>(socketDescriptors.size()));
    if (startupResult != RakNet::RAKNET_STARTED) {
        mRunning.store(false, std::memory_order_release);
        return false;
    }

    mPeer->SetMaximumIncomingConnections(static_cast<std::uint16_t>(maxConnections));
    mIoThread = std::jthread([this](std::stop_token token) { ioLoop(token); });
    return true;
}

void ServerNetworkSystem::stop() {
    if (!mRunning.exchange(false, std::memory_order_acq_rel)) {
        return;
    }

    if (mIoThread.joinable()) {
        mIoThread.request_stop();
        mIoWakeSignal.release();
        mIoThread.join();
    }

    auto oldSessions = mSessionsSnapshot.exchange(
        std::shared_ptr<const SessionMap>{std::make_shared<SessionMap>()},
        std::memory_order_acq_rel
    );
    for (const auto& [_, session] : *oldSessions) {
        if (session) {
            session->markDisconnected();
        }
    }

    if (mPeer) {
        mPeer->Shutdown(0);
    }

    mScheduledDueTimeByGuid.clear();
    mFlushSchedule       = {};
    mAdaptiveBudgetLevel = 0;
    mPromoteStreak       = 0;
    mDemoteStreak        = 0;

    std::uint64_t dirtyGuid{};
    while (mDirtySessionGuids.try_dequeue(dirtyGuid)) {}

    {
        std::scoped_lock lock{mPacketPumpMutex};
        mActivePacketPumps.clear();
    }
}

bool ServerNetworkSystem::isRunning() const noexcept { return mRunning.load(std::memory_order_acquire); }

bool ServerNetworkSystem::sendToClient(RakNet::RakNetGUID guid, std::span<const std::byte> buffer) {
    auto session = getSession(guid);
    if (!session || !session->sendPacket(buffer)) {
        return false;
    }

    if (session->tryMarkOutboundDirty()) {
        (void)mDirtySessionGuids.enqueue(guid.g);
    }

    notifyIoWorker();
    return true;
}

bool ServerNetworkSystem::sendToClient(RakNet::RakNetGUID guid, std::vector<std::byte>&& buffer) {
    auto session = getSession(guid);
    if (!session || !session->sendPacket(std::move(buffer))) {
        return false;
    }

    if (session->tryMarkOutboundDirty()) {
        (void)mDirtySessionGuids.enqueue(guid.g);
    }

    notifyIoWorker();
    return true;
}

std::uint32_t ServerNetworkSystem::sendToClientImmediately(RakNet::RakNetGUID guid, std::span<const std::byte> buffer) {
    auto session = getSession(guid);
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
    if (!mImmediateSends.enqueue(ImmediateSendRequest{guid.g, std::move(payload), receipt})) {
        return 0;
    }

    notifyIoWorker();

    return receipt;
}

std::uint32_t ServerNetworkSystem::sendToClientImmediately(RakNet::RakNetGUID guid, std::vector<std::byte>&& buffer) {
    auto session = getSession(guid);
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

    if (!mImmediateSends.enqueue(ImmediateSendRequest{guid.g, std::move(buffer), receipt})) {
        return 0;
    }

    notifyIoWorker();

    return receipt;
}

bool ServerNetworkSystem::receiveFromClient(RakNet::RakNetGUID guid, std::vector<std::byte>& outBuffer) noexcept {
    auto session = getSession(guid);
    return session ? session->receivePacket(outBuffer) : false;
}

coro::Task<Result<std::vector<std::byte>>> ServerNetworkSystem::receiveFromClientAsync(RakNet::RakNetGUID guid) {
    auto session = getSession(guid);
    if (!session) {
        co_return error_utils::makeError("no active session");
    }

    co_return co_await session->receivePacketAsync();
}

bool ServerNetworkSystem::getClientNetworkStatus(RakNet::RakNetGUID guid, NetworkStatus& outStatus) const noexcept {
    auto session = getSession(guid);
    if (!session) {
        return false;
    }

    outStatus = session->getNetworkStatus();
    return true;
}

bool ServerNetworkSystem::setOnConnected(RawEventCallback callback, void* userData) noexcept {
    return setOnEventRaw(mOnConnectedHandler, callback, userData, nullptr);
}

bool ServerNetworkSystem::setOnDisconnected(RawEventCallback callback, void* userData) noexcept {
    return setOnEventRaw(mOnDisconnectedHandler, callback, userData, nullptr);
}

bool ServerNetworkSystem::setOnConnectionFailed(RawEventCallback callback, void* userData) noexcept {
    return setOnEventRaw(mOnConnectionFailedHandler, callback, userData, nullptr);
}

bool ServerNetworkSystem::setOnPacketReceive(RawPacketReceiveCallback callback, void* userData) {
    return setOnPacketRaw(callback, userData, nullptr);
}

bool ServerNetworkSystem::setOnEventRaw(
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

bool ServerNetworkSystem::setOnPacketRaw(
    RawPacketReceiveCallback callback,
    void*                    userData,
    void (*destroyUserData)(void*) noexcept
) {
    if (!callback) {
        if (destroyUserData && userData) {
            destroyUserData(userData);
        }
        mOnPacketReceiveHandler.store(std::shared_ptr<PacketHook>{}, std::memory_order_release);
        auto sessions = mSessionsSnapshot.load(std::memory_order_acquire);
        for (const auto& [_, session] : *sessions) {
            if (session) {
                session->cancelReceiveWaiters();
            }
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

    startPacketPumpsForExistingSessions();
    return true;
}

void ServerNetworkSystem::startPacketPumpIfNeeded(RakNet::RakNetGUID guid) {
    auto packetHook = mOnPacketReceiveHandler.load(std::memory_order_acquire);
    if (!packetHook || !packetHook->mCallback) {
        return;
    }

    auto session = getSession(guid);
    if (!session || !session->isConnected()) {
        return;
    }

    const auto expectedGeneration = session->receivePumpGeneration();

    {
        std::scoped_lock lock{mPacketPumpMutex};
        auto [it, inserted] = mActivePacketPumps.insert(guid.g);
        (void)it;
        if (!inserted) {
            return;
        }
    }

    coro::startPacketPump(
        mScheduler,
        [session, expectedGeneration]() noexcept -> coro::Task<Result<std::vector<std::byte>>> {
            co_return co_await session->receivePacketAsync(expectedGeneration);
        },
        [this, guid](std::vector<std::byte>&& packet) noexcept -> bool {
            auto currentHook = mOnPacketReceiveHandler.load(std::memory_order_acquire);
            if (!currentHook || !currentHook->mCallback) {
                auto session = getSession(guid);
                if (session) {
                    (void)session->enqueueInboundPacket(std::move(packet));
                }
                return false;
            }

            currentHook->mCallback(currentHook->mUserData, guid, std::move(packet));
            return true;
        },
        [this, guid]() noexcept {
            {
                std::scoped_lock lock{mPacketPumpMutex};
                mActivePacketPumps.erase(guid.g);
            }

            auto currentHook = mOnPacketReceiveHandler.load(std::memory_order_acquire);
            if (!currentHook || !currentHook->mCallback) {
                return;
            }

            NetworkStatus status{};
            if (!getClientNetworkStatus(guid, status) || !status.getConnected()) {
                return;
            }

            startPacketPumpIfNeeded(guid);
        }
    );
}

void ServerNetworkSystem::startPacketPumpsForExistingSessions() {
    auto sessions = mSessionsSnapshot.load(std::memory_order_acquire);
    for (const auto& [guidValue, session] : *sessions) {
        if (!session || !session->isConnected()) {
            continue;
        }

        RakNet::RakNetGUID guid{};
        guid.g = guidValue;
        startPacketPumpIfNeeded(guid);
    }
}

std::uint64_t ServerNetworkSystem::droppedEventCallbackCount() const noexcept {
    return mDroppedEventCallbacks.load(std::memory_order_relaxed);
}

std::size_t ServerNetworkSystem::sessionCount() const {
    auto sessions = mSessionsSnapshot.load(std::memory_order_acquire);
    return sessions->size();
}

ServerNetworkSystem::SessionPtr ServerNetworkSystem::getSession(RakNet::RakNetGUID guid) const noexcept {
    auto sessions = mSessionsSnapshot.load(std::memory_order_acquire);
    auto it       = sessions->find(guid.g);
    if (it == sessions->end() || !it->second) {
        return nullptr;
    }

    return it->second;
}

void ServerNetworkSystem::ioLoop(std::stop_token stopToken) {
    while (!stopToken.stop_requested() && mRunning.load(std::memory_order_acquire)) {
        mSendWakeRequested.store(false, std::memory_order_release);
        const bool progressed = ioTickOnce();

        if (!progressed) {
            const auto receiveBudget =
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(RECEIVE_IDLE_SLEEP);
            auto waitBudget = receiveBudget;

            const auto nowNs = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                              std::chrono::steady_clock::now().time_since_epoch()
            )
                                                              .count());

            while (!mFlushSchedule.empty()) {
                const auto top = mFlushSchedule.top();
                auto       it  = mScheduledDueTimeByGuid.find(top.mGuid);
                if (it == mScheduledDueTimeByGuid.end() || it->second != top.mDueTimeNs) {
                    mFlushSchedule.pop();
                    continue;
                }

                if (top.mDueTimeNs <= nowNs) {
                    waitBudget = std::chrono::steady_clock::duration::zero();
                } else {
                    const auto untilDue = std::chrono::nanoseconds(top.mDueTimeNs - nowNs);
                    waitBudget          = std::min(
                        receiveBudget,
                        std::chrono::duration_cast<std::chrono::steady_clock::duration>(untilDue)
                    );
                }
                break;
            }

            if (waitBudget > std::chrono::steady_clock::duration::zero() && !stopToken.stop_requested()) {
                (void)mIoWakeSignal.try_acquire_for(waitBudget);
            }
        }
    }
}

bool ServerNetworkSystem::ioTickOnce() noexcept {
    bool progressed = false;

    for (RakNet::Packet* packet = mPeer->Receive(); packet != nullptr; packet = mPeer->Receive()) {
        progressed = true;
        processIncomingPacket(packet);
        mPeer->DeallocatePacket(packet);
    }

    flushOutboundPackets();
    return progressed;
}

void ServerNetworkSystem::processIncomingPacket(RakNet::Packet* packet) {
    if (!packet || !packet->data || packet->length == 0) {
        return;
    }

    const auto messageId = packet->data[0];
    const auto key       = packet->guid.g;

    if (messageId == ToMessageID(ID_NEW_INCOMING_CONNECTION)) {
        auto remote          = RakNet::AddressOrGUID{packet->guid};
        remote.systemAddress = packet->systemAddress;
        auto stableSession   = std::make_shared<Session>(mPeer.get(), remote, &mScheduler);

        for (;;) {
            auto current = mSessionsSnapshot.load(std::memory_order_acquire);
            auto next    = std::make_shared<SessionMap>(*current);
            (*next)[key] = stableSession;

            auto desired = std::const_pointer_cast<const SessionMap>(next);
            if (mSessionsSnapshot
                    .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
                break;
            }
        }

        emitEvent(NetworkEvent{NetworkEventType::Connected, packet->guid, packet->systemAddress});
        startPacketPumpIfNeeded(packet->guid);
        return;
    }

    if (messageId == ToMessageID(ID_DISCONNECTION_NOTIFICATION) || messageId == ToMessageID(ID_CONNECTION_LOST)) {
        mScheduledDueTimeByGuid.erase(key);

        SessionPtr removedSession;
        bool       removed = false;
        for (;;) {
            auto current = mSessionsSnapshot.load(std::memory_order_acquire);
            auto it      = current->find(key);
            if (it == current->end()) {
                break;
            }

            auto next = std::make_shared<SessionMap>(*current);
            auto jt   = next->find(key);
            if (jt != next->end()) {
                removedSession = jt->second;
                next->erase(jt);
            }

            auto desired = std::const_pointer_cast<const SessionMap>(next);
            if (mSessionsSnapshot
                    .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
                removed = static_cast<bool>(removedSession);
                break;
            }
        }

        if (removedSession) {
            removedSession->markDisconnected();
        }

        if (removed) {
            emitEvent(NetworkEvent{NetworkEventType::Disconnected, packet->guid, packet->systemAddress});
        }

        {
            std::scoped_lock lock{mPacketPumpMutex};
            mActivePacketPumps.erase(packet->guid.g);
        }
        return;
    }

    if (messageId != MINECRAFT_BATCH_PACKET_ID || packet->length <= 1) {
        return;
    }

    auto       sessions = mSessionsSnapshot.load(std::memory_order_acquire);
    auto       it       = sessions->find(key);
    SessionPtr session  = (it != sessions->end()) ? it->second : nullptr;

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

void ServerNetworkSystem::emitEvent(NetworkEvent event) {
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

void ServerNetworkSystem::flushOutboundPackets() {
    const auto flushStart = std::chrono::steady_clock::now();
    const auto nowNs      = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count()
    );
    const auto sendIntervalNs =
        static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(SEND_FLUSH_INTERVAL).count());

    auto scheduleGuid = [this](std::uint64_t guid, std::uint64_t dueTimeNs) {
        auto [it, inserted] = mScheduledDueTimeByGuid.insert_or_assign(guid, dueTimeNs);
        (void)it;
        (void)inserted;
        mFlushSchedule.push(FlushScheduleEntry{guid, dueTimeNs});
    };

    auto sessionsSnapshot = mSessionsSnapshot.load(std::memory_order_acquire);

    ImmediateSendRequest immediate;
    while (mImmediateSends.try_dequeue(immediate)) {
        auto it = sessionsSnapshot->find(immediate.mGuid);
        if (it != sessionsSnapshot->end() && it->second && it->second->isConnected()) {
            (void)it->second->sendPacketImmediately(immediate.mPayload, immediate.mForceReceiptNumber);
        }
    }

    std::uint64_t dirtyGuid{};
    while (mDirtySessionGuids.try_dequeue(dirtyGuid)) {
        const auto guidValue = dirtyGuid;
        if (mScheduledDueTimeByGuid.contains(guidValue)) {
            continue;
        }

        scheduleGuid(guidValue, nowNs);
    }

    const auto scheduledCount = mScheduledDueTimeByGuid.size();

    std::size_t                         maxSessionsThisPass{};
    std::chrono::steady_clock::duration maxFlushTimeThisPass{};

    if (scheduledCount <= LOW_LOAD_FAST_PATH_MAX_SCHEDULED) {
        // Keep low-load path branch-light and stable for the common 3-50 session range.
        mAdaptiveBudgetLevel = 0;
        mPromoteStreak       = 0;
        mDemoteStreak        = 0;

        maxSessionsThisPass  = std::min(LOW_LOAD_SESSION_BUDGET, HARD_MAX_SESSIONS_PER_FLUSH_PASS);
        maxFlushTimeThisPass = std::min(
            std::chrono::duration_cast<std::chrono::steady_clock::duration>(LOW_LOAD_TIME_BUDGET),
            std::chrono::duration_cast<std::chrono::steady_clock::duration>(HARD_MAX_FLUSH_TIME_BUDGET)
        );
    } else {
        const auto currentLevel = static_cast<std::size_t>(mAdaptiveBudgetLevel);

        if (currentLevel + 1 < ADAPTIVE_BUDGET_LEVELS && scheduledCount >= PROMOTE_AT_SCHEDULED[currentLevel]) {
            ++mPromoteStreak;
            mDemoteStreak = 0;
            if (mPromoteStreak >= PROMOTE_STREAK_THRESHOLD) {
                ++mAdaptiveBudgetLevel;
                mPromoteStreak = 0;
            }
        } else if (currentLevel > 0 && scheduledCount <= DEMOTE_AT_SCHEDULED[currentLevel]) {
            ++mDemoteStreak;
            mPromoteStreak = 0;
            if (mDemoteStreak >= DEMOTE_STREAK_THRESHOLD) {
                --mAdaptiveBudgetLevel;
                mDemoteStreak = 0;
            }
        } else {
            mPromoteStreak = 0;
            mDemoteStreak  = 0;
        }

        const auto adaptiveLevel = static_cast<std::size_t>(mAdaptiveBudgetLevel);
        maxSessionsThisPass      = std::min(SESSION_BUDGETS_PER_LEVEL[adaptiveLevel], HARD_MAX_SESSIONS_PER_FLUSH_PASS);
        maxFlushTimeThisPass     = std::min(
            TIME_BUDGETS_PER_LEVEL[adaptiveLevel],
            std::chrono::duration_cast<std::chrono::steady_clock::duration>(HARD_MAX_FLUSH_TIME_BUDGET)
        );
    }

    std::size_t processed = 0;
    while (processed < maxSessionsThisPass && (std::chrono::steady_clock::now() - flushStart) < maxFlushTimeThisPass) {
        bool madeProgress = false;
        while (!mFlushSchedule.empty()) {
            const auto top = mFlushSchedule.top();
            auto       it  = mScheduledDueTimeByGuid.find(top.mGuid);
            if (it == mScheduledDueTimeByGuid.end() || it->second != top.mDueTimeNs) {
                mFlushSchedule.pop();
                continue;
            }

            const auto loopNowNs = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                                  std::chrono::steady_clock::now().time_since_epoch()
            )
                                                                  .count());
            if (top.mDueTimeNs > loopNowNs) {
                return;
            }

            mFlushSchedule.pop();
            mScheduledDueTimeByGuid.erase(it);

            auto sessionIt = sessionsSnapshot->find(top.mGuid);
            if (sessionIt == sessionsSnapshot->end() || !sessionIt->second) {
                madeProgress = true;
                break;
            }

            auto session = sessionIt->second;
            if (!session->isConnected()) {
                madeProgress = true;
                break;
            }

            session->clearOutboundDirty();

            PreparedSend prepared{};
            prepareBatchedSendsForSession(session, prepared);
            if (!prepared.mPayload.empty()) {
                auto framed = prependMinecraftBatchHeader(prepared.mPayload);
                (void)session->sendRawPacketImmediately(framed, prepared.mForceReceiptNumber);
                gPacketBufferPool.release(std::move(framed));
            }

            if (session->hasPendingOutboundPackets()) {
                (void)session->tryMarkOutboundDirty();
                scheduleGuid(top.mGuid, loopNowNs + sendIntervalNs);
            }

            ++processed;
            madeProgress = true;
            break;
        }

        if (mFlushSchedule.empty()) {
            return;
        }

        if (!madeProgress) {
            return;
        }
    }
}

void ServerNetworkSystem::notifyIoWorker() noexcept {
    if (mSendWakeRequested.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    mIoWakeSignal.release();
}

void ServerNetworkSystem::RakPeerDeleter::operator()(RakNet::RakPeerInterface* peer) const noexcept {
    if (peer) {
        RakNet::RakPeerInterface::DestroyInstance(peer);
    }
}

} // namespace sculk::protocol::inline abi_v975
