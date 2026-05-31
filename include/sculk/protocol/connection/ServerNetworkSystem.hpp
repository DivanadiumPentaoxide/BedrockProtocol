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

namespace sculk::protocol::inline abi_v975 {

class ServerNetworkSystem final {
public:
    using RawEventCallback         = void (*)(void*, const NetworkEvent&) noexcept;
    using RawPacketReceiveCallback = void (*)(void*, RakNet::RakNetGUID, std::vector<std::byte>&&) noexcept;

public:
    // Usage:
    // 1) start(port, maxConnections)
    // 2) call sendToClient()/sendToClientImmediately() from worker threads
    // 3) poll receiveFromClient(guid, outBuffer) from your logic threads
    // 4) stop() on shutdown
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

    // Stop I/O loop and release all sessions.
    void stop();

    [[nodiscard]] bool isRunning() const noexcept;

    [[nodiscard]] bool sendToClient(RakNet::RakNetGUID guid, std::span<const std::byte> buffer);

    [[nodiscard]] bool sendToClient(RakNet::RakNetGUID guid, std::vector<std::byte>&& buffer);

    [[nodiscard]] std::uint32_t sendToClientImmediately(RakNet::RakNetGUID guid, std::span<const std::byte> buffer);

    [[nodiscard]] std::uint32_t sendToClientImmediately(RakNet::RakNetGUID guid, std::vector<std::byte>&& buffer);

    [[nodiscard]] bool receiveFromClient(RakNet::RakNetGUID guid, std::vector<std::byte>& outBuffer) noexcept;

    [[nodiscard]] coro::Task<Result<std::vector<std::byte>>> receiveFromClientAsync(RakNet::RakNetGUID guid);

    // Returns false when the target session does not exist.
    [[nodiscard]] bool getClientNetworkStatus(RakNet::RakNetGUID guid, NetworkStatus& outStatus) const noexcept;

    [[nodiscard]] std::size_t sessionCount() const;

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
        requires std::invocable<F&, RakNet::RakNetGUID, std::vector<std::byte>&&>
              && std::is_nothrow_invocable_v<F&, RakNet::RakNetGUID, std::vector<std::byte>&&>
              && std::is_nothrow_constructible_v<std::decay_t<F>, F&&>
              && std::is_nothrow_move_constructible_v<std::decay_t<F>>
              && std::is_nothrow_destructible_v<std::decay_t<F>>
    bool setOnPacketReceive(F&& handler) {
        return setOnPacketWithLambda(std::forward<F>(handler));
    }

    [[nodiscard]] std::uint64_t droppedEventCallbackCount() const noexcept;

private:
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

    [[nodiscard]] SessionPtr getSession(RakNet::RakNetGUID guid) const noexcept;

    void emitEvent(NetworkEvent event);

    void ioLoop(std::stop_token stopToken);

    [[nodiscard]] bool ioTickOnce() noexcept;

    void processIncomingPacket(RakNet::Packet* packet);

    void flushOutboundPackets();

    void notifyIoWorker() noexcept;

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
        requires std::invocable<F&, RakNet::RakNetGUID, std::vector<std::byte>&&>
              && std::is_nothrow_invocable_v<F&, RakNet::RakNetGUID, std::vector<std::byte>&&>
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
                [](void* userData, RakNet::RakNetGUID guid, std::vector<std::byte>&& packet) noexcept {
                    (*static_cast<Handler*>(userData))(guid, std::move(packet));
                },
                stored,
                [](void* userData) noexcept { delete static_cast<Handler*>(userData); }
            );
        }
    }

    void startPacketPumpIfNeeded(RakNet::RakNetGUID guid);

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
    std::uint8_t                             mAdaptiveBudgetLevel{0};
    std::uint8_t                             mPromoteStreak{0};
    std::uint8_t                             mDemoteStreak{0};
    std::counting_semaphore<>                mIoWakeSignal{0};
    std::atomic_bool                         mSendWakeRequested{false};
    std::atomic_uint32_t                     mNextImmediateReceipt{1};
    std::atomic_uint64_t                     mDroppedEventCallbacks{0};
    std::atomic<std::shared_ptr<EventHook>>  mOnConnectedHandler{};
    std::atomic<std::shared_ptr<EventHook>>  mOnDisconnectedHandler{};
    std::atomic<std::shared_ptr<EventHook>>  mOnConnectionFailedHandler{};
    std::atomic<std::shared_ptr<PacketHook>> mOnPacketReceiveHandler{};
    std::mutex                               mPacketPumpMutex;
    std::unordered_set<std::uint64_t>        mActivePacketPumps;
};

} // namespace sculk::protocol::inline abi_v975
