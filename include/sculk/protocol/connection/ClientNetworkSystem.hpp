// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "Session.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include "sculk/protocol/connection/thread/ThreadPool.hpp"
#include <RakPeerInterface.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace io {
class ClientIoRuntime;
}

class ClientNetworkSystem final {
public:
    using ConnectionEventCallback = std::function<void()>;
    using PacketReceiveCallback   = std::function<void(std::unique_ptr<IPacket>&& packet)>;

public:
    explicit ClientNetworkSystem(std::size_t workerThreadCount = 0);
    explicit ClientNetworkSystem(thread::ThreadPool& threadPool);
    ClientNetworkSystem(thread::ThreadPool& threadPool, io::ClientIoRuntime& ioRuntime);

    ClientNetworkSystem(const ClientNetworkSystem&)            = delete;
    ClientNetworkSystem& operator=(const ClientNetworkSystem&) = delete;
    ClientNetworkSystem(ClientNetworkSystem&&)                 = delete;
    ClientNetworkSystem& operator=(ClientNetworkSystem&&)      = delete;

    ~ClientNetworkSystem();

public:
    [[nodiscard]] bool connect(std::string_view host, std::uint16_t port, std::uint16_t localPort = 0);

    void disconnect();

    [[nodiscard]] bool isRunning() const noexcept;

    [[nodiscard]] bool isConnected() const noexcept;

    [[nodiscard]] bool sendPacket(const IPacket& packet);

    [[nodiscard]] bool sendPacketImmediately(const IPacket& packet);

    [[nodiscard]] std::unique_ptr<IPacket> receivePacket() noexcept;

    [[nodiscard]] coro::Task<Result<std::unique_ptr<IPacket>>> receivePacketAsync();

    [[nodiscard]] bool getServerNetworkStatus(NetworkStatus& outStatus) const noexcept;

    bool setOnConnected(ConnectionEventCallback callback) noexcept;

    bool setOnDisconnected(ConnectionEventCallback callback) noexcept;

    bool setOnPacketReceive(PacketReceiveCallback callback);

    void setPacketCallbackTakesOverInbound(bool enabled) noexcept;

    [[nodiscard]] Session& getSession() const noexcept;

    [[nodiscard]] bool getNetworkStatus(NetworkStatus& outStatus) const noexcept;

    [[nodiscard]] bool ioTickOnce() noexcept;

private:
    struct RakPeerDeleter {
        void operator()(RakNet::RakPeerInterface* peer) const noexcept;
    };

    struct CallbacksState {
        ConnectionEventCallback mOnConnected{};
        ConnectionEventCallback mOnDisconnected{};
        PacketReceiveCallback   mOnPacketReceive{};
    };

private:
    void receiveLoop(std::stop_token stopToken);

    void flushLoop(std::stop_token stopToken);

    void processIncomingPacket(RakNet::Packet* packet);

    void submitConnectedEvent() noexcept;

    void submitDisconnectedEvent() noexcept;

    void submitPacketEvent(std::unique_ptr<IPacket>&& packet) noexcept;

private:
    std::unique_ptr<RakNet::RakPeerInterface, RakPeerDeleter> mPeer{};
    std::unique_ptr<thread::ThreadPool>                       mOwnedThreadPool{};
    thread::ThreadPool*                                       mThreadPool{};
    std::atomic_bool                                          mRunning{false};
    std::jthread                                              mReceiveThread{};
    std::jthread                                              mFlushThread{};
    std::atomic<std::shared_ptr<Session>>                     mSession{};
    std::atomic<std::shared_ptr<const CallbacksState>>        mCallbacksState{};
    std::atomic_bool                                          mCallbackTakesOverInbound{true};
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
