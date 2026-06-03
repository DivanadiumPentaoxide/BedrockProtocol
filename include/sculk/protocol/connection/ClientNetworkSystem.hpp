// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "Session.hpp"
#include "sculk/protocol/connection/coro/Scheduler.hpp"
#include "sculk/protocol/connection/io/ClientIoRuntime.hpp"
#include "sculk/protocol/connection/thread/ThreadPool.hpp"
#include <RakPeerInterface.h>
#include <atomic>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <new>
#include <semaphore>
#include <span>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class ClientNetworkSystem final {
public:
    using ConnectionEventCallback = std::function<void()>;
    using PacketReceiveCallback   = std::function<void(std::unique_ptr<IPacket>&& packet)>;

public:
    explicit ClientNetworkSystem(std::size_t workerThreadCount = 0);
    explicit ClientNetworkSystem(thread::ThreadPool& threadPool);
    explicit ClientNetworkSystem(io::ClientIoRuntime& ioRuntime, std::size_t workerThreadCount = 0);
    explicit ClientNetworkSystem(thread::ThreadPool& threadPool, io::ClientIoRuntime& ioRuntime);

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

    const Session& getSession() const noexcept { return *mSession.load(std::memory_order_acquire); }

    Session& getSession() noexcept { return *mSession.load(std::memory_order_acquire); }

    // Stop I/O loop and close active connection.
    void disconnect();

    [[nodiscard]] bool isConnected() const noexcept;

    [[nodiscard]] bool sendPacket(const IPacket& packet);

    [[nodiscard]] std::uint32_t sendPacketImmediately(const IPacket& packet);

    [[nodiscard]] bool sendBuffer(std::span<const std::byte> buffer);

    [[nodiscard]] std::uint32_t sendBufferImmediately(std::span<const std::byte> buffer);

    [[nodiscard]] bool receiveBuffer(std::vector<std::byte>& outBuffer) noexcept;

    [[nodiscard]] coro::Task<Result<std::vector<std::byte>>> receiveBufferAsync();

    // Returns false when no active session is available.
    [[nodiscard]] bool getNetworkStatus(NetworkStatus& outStatus) const noexcept;

    bool setOnConnected(ConnectionEventCallback&& callback) noexcept;

    bool setOnDisconnected(ConnectionEventCallback&& callback) noexcept;

    bool setOnPacketReceive(PacketReceiveCallback&& callback);

    [[nodiscard]] std::uint64_t droppedEventCallbackCount() const noexcept;

private:
    friend class io::ClientIoRuntime;

    struct RakPeerDeleter {
        void operator()(RakNet::RakPeerInterface* peer) const noexcept;
    };

    using SessionPtr = std::shared_ptr<Session>;
    struct ImmediateSendRequest {
        SessionPtr    mSession{};
        PacketBuffer  mPayload{};
        std::uint32_t mForceReceiptNumber{};
    };

    void ioLoop(std::stop_token stopToken);

    void processIncomingPacket(RakNet::Packet* packet);

    void flushOutboundPackets();

    [[nodiscard]] bool ioTickOnce() noexcept;

    void notifyIoWorker() noexcept;

    void startPacketPumpIfNeeded();

private:
    std::unique_ptr<RakNet::RakPeerInterface, RakPeerDeleter> mPeer{};
    std::unique_ptr<thread::ThreadPool>                       mOwnedThreadPool{};
    thread::ThreadPool*                                       mThreadPool{};
    io::ClientIoRuntime*                                      mIoRuntime{};
    coro::Scheduler                                           mScheduler;
    std::atomic_bool                                          mRunning{false};
    bool                                                      mUsesSharedIoRuntime{false};
    std::jthread                                              mIoThread{};
    std::chrono::steady_clock::time_point                     mLastFlushTime{};
    std::atomic<SessionPtr>                                   mSession{};
    moodycamel::ConcurrentQueue<ImmediateSendRequest>         mImmediateSends{};
    std::counting_semaphore<>                                 mIoWakeSignal{0};
    std::atomic_bool                                          mSendWakeRequested{false};
    std::atomic_uint32_t                                      mNextImmediateReceipt{1};
    std::atomic_uint64_t                                      mDroppedEventCallbacks{0};
    std::atomic<std::shared_ptr<ConnectionEventCallback>>     mOnConnectedHandler{};
    std::atomic<std::shared_ptr<ConnectionEventCallback>>     mOnDisconnectedHandler{};
    std::atomic<std::shared_ptr<PacketReceiveCallback>>       mOnPacketReceiveHandler{};
    std::atomic_bool                                          mPacketPumpActive{false};
    std::atomic_bool                                          mRegisteredInSharedIoRuntime{false};
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
