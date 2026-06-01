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

namespace sculk::protocol::inline abi_v975 {

class ClientNetworkSystem final {
public:
    using RawEventCallback         = void (*)(void*, const NetworkEvent&) noexcept;
    using RawPacketReceiveCallback = void (*)(void*, std::vector<std::byte>&&) noexcept;

public:
    // Usage:
    // 1) connect(host, port)
    // 2) sendPacketBuffer() for queued send, or sendPacketBufferImmediately() for direct send
    // 3) poll receivePacketBuffer(outBuffer) in your game loop
    // 4) disconnect() on shutdown
    explicit ClientNetworkSystem(std::size_t workerThreadCount = 0);
    explicit ClientNetworkSystem(thread::ThreadPool& threadPool);
    ClientNetworkSystem(io::ClientIoRuntime& ioRuntime, std::size_t workerThreadCount = 0);
    ClientNetworkSystem(thread::ThreadPool& threadPool, io::ClientIoRuntime& ioRuntime);

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

    [[nodiscard]] bool sendBuffer(std::span<const std::byte> buffer);

    [[nodiscard]] std::uint32_t sendBufferImmediately(std::span<const std::byte> buffer);

    [[nodiscard]] bool receiveBuffer(std::vector<std::byte>& outBuffer) noexcept;

    [[nodiscard]] coro::Task<Result<std::vector<std::byte>>> receiveBufferAsync();

    // Returns false when no active session is available.
    [[nodiscard]] bool getNetworkStatus(NetworkStatus& outStatus) const noexcept;

    bool setOnConnected(RawEventCallback callback, void* userData = nullptr) noexcept;

    template <typename F>
        requires std::invocable<F&, const NetworkEvent&> && std::is_nothrow_invocable_v<F&, const NetworkEvent&>
              && std::is_nothrow_constructible_v<std::decay_t<F>, F&&>
              && std::is_nothrow_move_constructible_v<std::decay_t<F>>
              && std::is_nothrow_destructible_v<std::decay_t<F>>
    bool setOnConnected(F&& handler) {
        return setOnEventWithLambda(mOnConnectedHandler, std::forward<F>(handler));
    }

    bool setOnDisconnected(RawEventCallback callback, void* userData = nullptr) noexcept;

    template <typename F>
        requires std::invocable<F&, const NetworkEvent&> && std::is_nothrow_invocable_v<F&, const NetworkEvent&>
              && std::is_nothrow_constructible_v<std::decay_t<F>, F&&>
              && std::is_nothrow_move_constructible_v<std::decay_t<F>>
              && std::is_nothrow_destructible_v<std::decay_t<F>>
    bool setOnDisconnected(F&& handler) {
        return setOnEventWithLambda(mOnDisconnectedHandler, std::forward<F>(handler));
    }

    bool setOnConnectionFailed(RawEventCallback callback, void* userData = nullptr) noexcept;

    template <typename F>
        requires std::invocable<F&, const NetworkEvent&> && std::is_nothrow_invocable_v<F&, const NetworkEvent&>
              && std::is_nothrow_constructible_v<std::decay_t<F>, F&&>
              && std::is_nothrow_move_constructible_v<std::decay_t<F>>
              && std::is_nothrow_destructible_v<std::decay_t<F>>
    bool setOnConnectionFailed(F&& handler) {
        return setOnEventWithLambda(mOnConnectionFailedHandler, std::forward<F>(handler));
    }

    bool setOnPacketReceive(RawPacketReceiveCallback callback, void* userData = nullptr);

    template <typename F>
        requires std::invocable<F&, std::vector<std::byte>&&>
              && std::is_nothrow_invocable_v<F&, std::vector<std::byte>&&>
              && std::is_nothrow_constructible_v<std::decay_t<F>, F&&>
              && std::is_nothrow_move_constructible_v<std::decay_t<F>>
              && std::is_nothrow_destructible_v<std::decay_t<F>>
    bool setOnPacketReceive(F&& handler) {
        return setOnPacketWithLambda(std::forward<F>(handler));
    }

    [[nodiscard]] std::uint64_t droppedEventCallbackCount() const noexcept;

private:
    friend class io::ClientIoRuntime;

    struct EventHook {
        RawEventCallback mCallback{};
        void*            mUserData{};
        void (*mDestroyUserData)(void*) noexcept {};

        ~EventHook() noexcept {
            if (mDestroyUserData && mUserData) {
                mDestroyUserData(mUserData);
            }
        }
    };

    struct PacketHook {
        RawPacketReceiveCallback mCallback{};
        void*                    mUserData{};
        void (*mDestroyUserData)(void*) noexcept {};

        ~PacketHook() noexcept {
            if (mDestroyUserData && mUserData) {
                mDestroyUserData(mUserData);
            }
        }
    };

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

    void emitEvent(NetworkEvent event);

    bool setOnEventRaw(
        std::atomic<std::shared_ptr<EventHook>>& target,
        RawEventCallback                         callback,
        void*                                    userData,
        void (*destroyUserData)(void*) noexcept
    ) noexcept;

    bool setOnPacketRaw(RawPacketReceiveCallback callback, void* userData, void (*destroyUserData)(void*) noexcept);

    template <typename F>
        requires std::invocable<F&, const NetworkEvent&> && std::is_nothrow_invocable_v<F&, const NetworkEvent&>
    bool setOnEventWithLambda(std::atomic<std::shared_ptr<EventHook>>& target, F&& handler) {
        using Handler = std::decay_t<F>;

        if constexpr (std::is_convertible_v<Handler, RawEventCallback>) {
            return setOnEventRaw(target, static_cast<RawEventCallback>(handler), nullptr, nullptr);
        } else {
            static_assert(
                std::is_nothrow_constructible_v<Handler, F&&>,
                "event handler must be nothrow-constructible for no-exception mode"
            );
            static_assert(
                std::is_nothrow_destructible_v<Handler>,
                "event handler must be nothrow-destructible for no-exception mode"
            );
            static_assert(
                std::is_nothrow_move_constructible_v<Handler>,
                "event handler must be nothrow-move-constructible for no-exception mode"
            );

            auto* stored = new (std::nothrow) Handler(std::forward<F>(handler));
            if (!stored) {
                return false;
            }

            return setOnEventRaw(
                target,
                [](void* userData, const NetworkEvent& event) noexcept { (*static_cast<Handler*>(userData))(event); },
                stored,
                [](void* userData) noexcept { delete static_cast<Handler*>(userData); }
            );
        }
    }

    template <typename F>
        requires std::invocable<F&, std::vector<std::byte>&&>
              && std::is_nothrow_invocable_v<F&, std::vector<std::byte>&&>
    bool setOnPacketWithLambda(F&& handler) {
        using Handler = std::decay_t<F>;

        if constexpr (std::is_convertible_v<Handler, RawPacketReceiveCallback>) {
            return setOnPacketRaw(static_cast<RawPacketReceiveCallback>(handler), nullptr, nullptr);
        } else {
            static_assert(
                std::is_nothrow_constructible_v<Handler, F&&>,
                "packet handler must be nothrow-constructible for no-exception mode"
            );
            static_assert(
                std::is_nothrow_destructible_v<Handler>,
                "packet handler must be nothrow-destructible for no-exception mode"
            );
            static_assert(
                std::is_nothrow_move_constructible_v<Handler>,
                "packet handler must be nothrow-move-constructible for no-exception mode"
            );

            auto* stored = new (std::nothrow) Handler(std::forward<F>(handler));
            if (!stored) {
                return false;
            }

            return setOnPacketRaw(
                [](void* userData, std::vector<std::byte>&& packet) noexcept {
                    (*static_cast<Handler*>(userData))(std::move(packet));
                },
                stored,
                [](void* userData) noexcept { delete static_cast<Handler*>(userData); }
            );
        }
    }

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
    std::atomic<std::shared_ptr<EventHook>>                   mOnConnectedHandler{};
    std::atomic<std::shared_ptr<EventHook>>                   mOnDisconnectedHandler{};
    std::atomic<std::shared_ptr<EventHook>>                   mOnConnectionFailedHandler{};
    std::atomic<std::shared_ptr<PacketHook>>                  mOnPacketReceiveHandler{};
    std::atomic_bool                                          mPacketPumpActive{false};
    std::atomic_bool                                          mRegisteredInSharedIoRuntime{false};
};

} // namespace sculk::protocol::inline abi_v975
