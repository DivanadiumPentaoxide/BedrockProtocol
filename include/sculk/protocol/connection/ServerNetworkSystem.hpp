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
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
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
    [[nodiscard]] bool start(std::uint16_t ipv4Port, std::uint32_t maxConnections);

    [[nodiscard]] bool start(std::uint16_t ipv4Port, std::uint16_t ipv6Port, std::uint32_t maxConnections);

    void setMotd(std::string_view motd);

    void updateAnnouncement() noexcept;

    void setMaxConnections(std::uint32_t maxConnections);

    void stop();

    [[nodiscard]] bool isRunning() const noexcept;

    [[nodiscard]] bool sendPacket(const RakNet::RakNetGUID& guid, const IPacket& packet);

    [[nodiscard]] bool sendPacketImmediately(const RakNet::RakNetGUID& guid, const IPacket& packet);

    [[nodiscard]] std::unique_ptr<IPacket> receivePacket(const RakNet::RakNetGUID& guid) noexcept;

    [[nodiscard]] coro::Task<Result<std::unique_ptr<IPacket>>> receivePacketAsync(const RakNet::RakNetGUID& guid);

    [[nodiscard]] bool getClientNetworkStatus(const RakNet::RakNetGUID& guid, NetworkStatus& outStatus) const noexcept;

    [[nodiscard]] std::size_t getSessionsCount() const;

    bool setOnConnected(ConnectionEventCallback&& callback) noexcept;

    bool setOnDisconnected(ConnectionEventCallback&& callback) noexcept;

    bool setOnNetworkEvent(ConnectionEventCallback&& callback) noexcept;

    bool setOnPacketReceive(PacketReceiveCallback&& callback);

    void setPacketCallbackTakesOverInbound(bool enabled) noexcept;

    [[nodiscard]] std::shared_ptr<Session> getSession(const RakNet::RakNetGUID& guid) const noexcept;

private:
    struct RakPeerDeleter {
        void operator()(RakNet::RakPeerInterface* peer) const noexcept;
    };

    struct SessionsState {
        std::unordered_map<std::uint64_t, std::shared_ptr<Session>> mByGuid{};
        std::vector<std::shared_ptr<Session>>                       mOrdered{};
    };

    struct CallbacksState {
        ConnectionEventCallback mOnConnected{};
        ConnectionEventCallback mOnDisconnected{};
        ConnectionEventCallback mOnNetworkEvent{};
        PacketReceiveCallback   mOnPacketReceive{};
    };

private:
    void receiveLoop(std::stop_token stopToken);

    void flushLoop(std::stop_token stopToken);

    void processIncomingPacket(RakNet::Packet* packet);

    void upsertSession(std::uint64_t key, std::shared_ptr<Session> session);

    std::shared_ptr<Session> removeSession(std::uint64_t key);

    void submitConnectedEvent(const RakNet::RakNetGUID& guid, const RakNet::SystemAddress& address) noexcept;

    void submitDisconnectedEvent(const RakNet::RakNetGUID& guid, const RakNet::SystemAddress& address) noexcept;

    void submitPacketEvent(
        const RakNet::RakNetGUID&    guid,
        const RakNet::SystemAddress& address,
        std::unique_ptr<IPacket>&&   packet
    ) noexcept;

private:
    std::uint32_t                                             mMaxConnections{};
    std::uint16_t                                             mIpv4Port{};
    std::optional<std::uint16_t>                              mIpv6Port{};
    std::string                                               mMotd{};
    std::unique_ptr<RakNet::RakPeerInterface, RakPeerDeleter> mPeer{};
    std::unique_ptr<thread::ThreadPool>                       mOwnedThreadPool{};
    thread::ThreadPool*                                       mThreadPool{};
    std::atomic_bool                                          mRunning{false};
    std::jthread                                              mReceiveThread{};
    std::jthread                                              mFlushThread{};
    std::atomic<std::shared_ptr<const SessionsState>>         mSessionsState{};
    std::atomic<std::shared_ptr<const CallbacksState>>        mCallbacksState{};
    std::atomic_bool                                          mCallbackTakesOverInbound{true};
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
