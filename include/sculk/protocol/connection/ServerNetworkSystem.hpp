// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "Session.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include "sculk/protocol/connection/coro/Scheduler.hpp"
#include "sculk/protocol/connection/thread/ThreadPool.hpp"
#include <RakPeerInterface.h>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <new>
#include <parallel_hashmap/phmap.h>
#include <queue>
#include <semaphore>
#include <span>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class ServerNetworkSystem final {
public:
    using ConnectionEventCallback =
        std::function<void(const RakNet::RakNetGUID& guid, const RakNet::SystemAddress& address)>;
    using PacketReceiveCallback = std::function<
        void(const RakNet::RakNetGUID& guid, const RakNet::SystemAddress& address, std::unique_ptr<IPacket>&& packet)>;

public:
    explicit ServerNetworkSystem(std::size_t workerThreadCount = 0);
    explicit ServerNetworkSystem(thread::ThreadPool& threadPool);

    ServerNetworkSystem(const ServerNetworkSystem&)            = delete;
    ServerNetworkSystem& operator=(const ServerNetworkSystem&) = delete;
    ServerNetworkSystem(ServerNetworkSystem&&)                 = delete;
    ServerNetworkSystem& operator=(ServerNetworkSystem&&)      = delete;

    ~ServerNetworkSystem();

public:
    // Start listening and spawn I/O loop.
    [[nodiscard]] bool start(std::uint16_t ipv4Port, std::uint32_t maxConnections);

    [[nodiscard]] bool start(std::uint16_t ipv4Port, std::uint16_t ipv6Port, std::uint32_t maxConnections);

    void setMotd(std::string_view motd);

    void setMaxConnections(std::uint32_t maxConnections);

    // Stop I/O loop and release all sessions.
    void stop();

    [[nodiscard]] bool isRunning() const noexcept;

    [[nodiscard]] bool sendPacket(const RakNet::RakNetGUID& guid, const IPacket& packet);

    [[nodiscard]] std::uint32_t sendPacketImmediately(const RakNet::RakNetGUID& guid, const IPacket& packet);

    [[nodiscard]] bool sendBuffer(const RakNet::RakNetGUID& guid, std::span<const std::byte> buffer);

    [[nodiscard]] bool sendBuffer(const RakNet::RakNetGUID& guid, std::vector<std::byte>&& buffer);

    [[nodiscard]] std::uint32_t
    sendBufferImmediately(const RakNet::RakNetGUID& guid, std::span<const std::byte> buffer);

    [[nodiscard]] std::uint32_t sendBufferImmediately(const RakNet::RakNetGUID& guid, std::vector<std::byte>&& buffer);

    [[nodiscard]] bool receiveBuffer(const RakNet::RakNetGUID& guid, std::vector<std::byte>& outBuffer) noexcept;

    [[nodiscard]] coro::Task<Result<std::vector<std::byte>>> receiveBufferAsync(const RakNet::RakNetGUID& guid);

    // Returns false when the target session does not exist.
    [[nodiscard]] bool getClientNetworkStatus(const RakNet::RakNetGUID& guid, NetworkStatus& outStatus) const noexcept;

    [[nodiscard]] std::size_t getSessionsCount() const;

    bool setOnConnected(ConnectionEventCallback&& callback) noexcept;

    bool setOnDisconnected(ConnectionEventCallback&& callback) noexcept;

    bool setOnNetworkEvent(ConnectionEventCallback&& callback) noexcept;

    bool setOnPacketReceive(PacketReceiveCallback&& callback);

    [[nodiscard]] std::uint64_t getDroppedEventCallbackCount() const noexcept;

private:
    struct RakPeerDeleter {
        void operator()(RakNet::RakPeerInterface* peer) const noexcept;
    };

    using SessionPtr = std::shared_ptr<Session>;
    using SessionMap = phmap::flat_hash_map<std::uint64_t, SessionPtr>;

    struct ImmediateSendRequest {
        std::uint64_t mGuid{};
        PacketBuffer  mPayload{};
        std::uint32_t mForceReceiptNumber{};
    };

    struct FlushScheduleEntry {
        std::uint64_t mGuid{};
        std::uint64_t mDueTimeNs{};
    };

    struct FlushScheduleCompare {
        [[nodiscard]] bool operator()(const FlushScheduleEntry& lhs, const FlushScheduleEntry& rhs) const noexcept {
            return lhs.mDueTimeNs > rhs.mDueTimeNs;
        }
    };

    [[nodiscard]] SessionPtr getSession(const RakNet::RakNetGUID& guid) const noexcept;

    void ioLoop(std::stop_token stopToken);

    [[nodiscard]] bool ioTickOnce() noexcept;

    void processIncomingPacket(RakNet::Packet* packet);

    void flushOutboundPackets();

    void notifyIoWorker() noexcept;

    void startPacketPumpIfNeeded(const RakNet::RakNetGUID& guid, const RakNet::SystemAddress& address);

    void startPacketPumpsForExistingSessions();

private:
    std::unique_ptr<RakNet::RakPeerInterface, RakPeerDeleter> mPeer{};
    std::unique_ptr<thread::ThreadPool>                       mOwnedThreadPool{};
    thread::ThreadPool*                                       mThreadPool{};
    coro::Scheduler                                           mScheduler;
    std::atomic_bool                                          mRunning{false};
    std::jthread                                              mIoThread{};
    std::atomic<std::shared_ptr<const SessionMap>>            mSessionsSnapshot{
        std::shared_ptr<const SessionMap>{std::make_shared<SessionMap>()}
    };
    moodycamel::ConcurrentQueue<ImmediateSendRequest>  mImmediateSends{};
    moodycamel::ConcurrentQueue<std::uint64_t>         mDirtySessionGuids{};
    phmap::flat_hash_map<std::uint64_t, std::uint64_t> mScheduledDueTimeByGuid{};
    std::priority_queue<FlushScheduleEntry, std::vector<FlushScheduleEntry>, FlushScheduleCompare> mFlushSchedule{};
    std::uint8_t                                          mAdaptiveBudgetLevel{};
    std::uint8_t                                          mPromoteStreak{};
    std::uint8_t                                          mDemoteStreak{};
    std::counting_semaphore<>                             mIoWakeSignal{0};
    std::atomic_bool                                      mSendWakeRequested{};
    std::atomic_uint32_t                                  mNextImmediateReceipt{1};
    std::atomic_uint64_t                                  mDroppedEventCallbacks{};
    std::atomic<std::shared_ptr<ConnectionEventCallback>> mOnConnectedHandler{};
    std::atomic<std::shared_ptr<ConnectionEventCallback>> mOnDisconnectedHandler{};
    std::atomic<std::shared_ptr<ConnectionEventCallback>> mOnNetworkEventHandler{};
    std::atomic<std::shared_ptr<PacketReceiveCallback>>   mOnPacketReceiveHandler{};
    std::mutex                                            mPacketPumpMutex{};
    std::unordered_set<std::uint64_t>                     mActivePacketPumps{};
    std::uint32_t                                         mMaxConnections{};
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
