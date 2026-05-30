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
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace sculk::protocol::inline abi_v975 {

class ClientNetworkSystem final {
public:
    // Usage:
    // 1) connect(host, port)
    // 2) sendPacket() for queued send, or sendPacketImmediately() for direct send
    // 3) poll receivePacket(outBuffer) in your game loop
    // 4) disconnect() on shutdown
    explicit ClientNetworkSystem(std::size_t workerThreadCount = 0);

    ClientNetworkSystem(const ClientNetworkSystem&)            = delete;
    ClientNetworkSystem& operator=(const ClientNetworkSystem&) = delete;
    ClientNetworkSystem(ClientNetworkSystem&&)                 = delete;
    ClientNetworkSystem& operator=(ClientNetworkSystem&&)      = delete;

    ~ClientNetworkSystem();

public:
    // Create local peer, connect to remote, and start I/O loop.
    [[nodiscard]] bool connect(
        const std::string& host,
        std::uint16_t      remotePort,
        std::uint16_t      localPort              = 0,
        std::uint32_t      connectionAttemptCount = 12,
        std::uint16_t      socketFamily           = AF_INET
    );

    // Stop I/O loop and close active connection.
    void disconnect();

    [[nodiscard]] bool isConnected() const noexcept;

    [[nodiscard]] bool sendPacket(std::span<const std::byte> buffer) noexcept;

    [[nodiscard]] std::uint32_t sendPacketImmediately(std::span<const std::byte> buffer) noexcept;

    [[nodiscard]] bool receivePacket(std::vector<std::byte>& outBuffer) noexcept;

    // Returns false when no active session is available.
    [[nodiscard]] bool getNetworkStatus(NetworkStatus& outStatus) const noexcept;

    std::uint64_t subscribeEvents(std::function<void(const NetworkEvent&)> handler);

    [[nodiscard]] bool unsubscribeEvents(std::uint64_t subscriptionId);

private:
    struct RakPeerDeleter {
        void operator()(RakNet::RakPeerInterface* peer) const noexcept;
    };

    using SessionPtr = std::shared_ptr<Session>;
    struct ImmediateSendRequest {
        SessionPtr    mSession{};
        PacketBuffer  mPayload{};
        std::uint32_t mForceReceiptNumber{};
    };
    using EventHandler    = std::pair<std::uint64_t, std::function<void(const NetworkEvent&)>>;
    using EventHandlerVec = std::vector<EventHandler>;

    void ioLoop(std::stop_token stopToken);

    void processIncomingPacket(RakNet::Packet* packet);

    void flushOutboundPackets();

    void emitEvent(NetworkEvent event);

private:
    std::unique_ptr<RakNet::RakPeerInterface, RakPeerDeleter> mPeer{};
    thread::ThreadPool                                        mThreadPool;
    coro::Scheduler                                           mScheduler;
    std::atomic_bool                                          mRunning{false};
    std::jthread                                              mIoThread{};
    std::atomic<SessionPtr>                                   mSession{};
    moodycamel::ConcurrentQueue<ImmediateSendRequest>         mImmediateSends{};
    std::mutex                                                mIoWaitMutex{};
    std::condition_variable                                   mIoWaitCv{};
    std::atomic_bool                                          mSendWakeRequested{false};
    std::atomic_uint32_t                                      mNextImmediateReceipt{1};
    std::atomic_uint64_t                                      mNextSubscriptionId{1};
    std::atomic<std::shared_ptr<const EventHandlerVec>>       mEventHandlersSnapshot{
        std::shared_ptr<const EventHandlerVec>{std::make_shared<EventHandlerVec>()}
    };
};

} // namespace sculk::protocol::inline abi_v975
