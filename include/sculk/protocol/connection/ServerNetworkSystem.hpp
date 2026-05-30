// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "NetworkEvent.hpp"
#include "Session.hpp"
#include "sculk/protocol/connection/coro/Scheduler.hpp"
#include "sculk/protocol/connection/thread/ThreadPool.hpp"
#include <RakPeerInterface.h>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <parallel_hashmap/phmap.h>
#include <span>
#include <thread>
#include <utility>
#include <vector>

namespace sculk::protocol::inline abi_v975 {

class ServerNetworkSystem final {
public:
    // Usage:
    // 1) start(port, maxConnections)
    // 2) call sendToClient()/sendToClientImmediately() from worker threads
    // 3) poll receiveFromClient(guid, outBuffer) from your logic threads
    // 4) stop() on shutdown
    explicit ServerNetworkSystem(std::size_t workerThreadCount = 0);

    ServerNetworkSystem(const ServerNetworkSystem&)            = delete;
    ServerNetworkSystem& operator=(const ServerNetworkSystem&) = delete;
    ServerNetworkSystem(ServerNetworkSystem&&)                 = delete;
    ServerNetworkSystem& operator=(ServerNetworkSystem&&)      = delete;

    ~ServerNetworkSystem();

public:
    // Start listening and spawn I/O loop.
    [[nodiscard]] bool start(std::uint16_t ipv4Port, std::uint32_t maxConnections);

    [[nodiscard]] bool start(std::uint16_t ipv4Port, std::uint16_t ipv6Port, std::uint32_t maxConnections);

    // Stop I/O loop and release all sessions.
    void stop();

    [[nodiscard]] bool isRunning() const noexcept;

    [[nodiscard]] bool sendToClient(RakNet::RakNetGUID guid, std::span<const std::byte> buffer) noexcept;

    [[nodiscard]] std::uint32_t
    sendToClientImmediately(RakNet::RakNetGUID guid, std::span<const std::byte> buffer) noexcept;

    [[nodiscard]] bool receiveFromClient(RakNet::RakNetGUID guid, std::vector<std::byte>& outBuffer) noexcept;

    // Returns false when the target session does not exist.
    [[nodiscard]] bool getClientNetworkStatus(RakNet::RakNetGUID guid, NetworkStatus& outStatus) const noexcept;

    [[nodiscard]] std::size_t sessionCount() const;

    std::uint64_t subscribeEvents(std::function<void(const NetworkEvent&)> handler);

    [[nodiscard]] bool unsubscribeEvents(std::uint64_t subscriptionId);

private:
    struct RakPeerDeleter {
        void operator()(RakNet::RakPeerInterface* peer) const noexcept;
    };

    using SessionPtr = std::shared_ptr<Session>;
    using SessionMap = phmap::flat_hash_map<std::uint64_t, SessionPtr>;
    struct ImmediateSendRequest {
        SessionPtr    mSession{};
        PacketBuffer  mPayload{};
        std::uint32_t mForceReceiptNumber{};
    };
    using EventHandler    = std::pair<std::uint64_t, std::function<void(const NetworkEvent&)>>;
    using EventHandlerVec = std::vector<EventHandler>;

    [[nodiscard]] SessionPtr getSession(RakNet::RakNetGUID guid) const noexcept;

    void emitEvent(NetworkEvent event);

    void ioLoop(std::stop_token stopToken);

    void processIncomingPacket(RakNet::Packet* packet);

    void flushOutboundPackets();

private:
    std::unique_ptr<RakNet::RakPeerInterface, RakPeerDeleter> mPeer{};
    thread::ThreadPool                                        mThreadPool;
    coro::Scheduler                                           mScheduler;
    std::atomic_bool                                          mRunning{false};
    std::jthread                                              mIoThread{};
    std::atomic<std::shared_ptr<const SessionMap>>            mSessionsSnapshot{
        std::shared_ptr<const SessionMap>{std::make_shared<SessionMap>()}
    };
    moodycamel::ConcurrentQueue<ImmediateSendRequest>   mImmediateSends{};
    moodycamel::ConcurrentQueue<SessionPtr>             mDirtySessions{};
    std::mutex                                          mIoWaitMutex{};
    std::condition_variable                             mIoWaitCv{};
    std::atomic_bool                                    mSendWakeRequested{false};
    std::atomic_uint32_t                                mNextImmediateReceipt{1};
    std::atomic_uint64_t                                mNextSubscriptionId{1};
    std::atomic<std::shared_ptr<const EventHandlerVec>> mEventHandlersSnapshot{
        std::shared_ptr<const EventHandlerVec>{std::make_shared<EventHandlerVec>()}
    };
};

} // namespace sculk::protocol::inline abi_v975
