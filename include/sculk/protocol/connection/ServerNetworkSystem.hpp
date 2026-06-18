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
#include <parallel_hashmap/phmap.h>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace detail {
template <
    class K,
    class V,
    class Hash  = phmap::priv::hash_default_hash<K>,
    class Eq    = phmap::priv::hash_default_eq<K>,
    class Alloc = phmap::priv::Allocator<phmap::priv::Pair<const K, V>>,
    size_t N    = 6>
using ThreadSafeParallelFlatHashMap = phmap::parallel_flat_hash_map<K, V, Hash, Eq, Alloc, N, std::shared_mutex>;
}

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
    [[nodiscard]] bool start(std::uint16_t ipv4Port, std::uint16_t ipv6Port, std::uint32_t maxConnections);

    void setMotd(std::string_view motd);

    void updateAnnouncement() noexcept;

    void setMaxConnections(std::uint32_t maxConnections);

    void stop();

    [[nodiscard]] bool isRunning() const noexcept;

    [[nodiscard]] bool getClientNetworkStatus(const RakNet::RakNetGUID& guid, NetworkStatus& outStatus) const noexcept;

    [[nodiscard]] std::size_t getSessionsCount() const;

    bool setOnConnected(ConnectionEventCallback&& callback) noexcept;

    bool setOnDisconnected(ConnectionEventCallback&& callback) noexcept;

    bool setOnPacketReceive(PacketReceiveCallback&& callback);

    [[nodiscard]] std::weak_ptr<Session> getSession(const RakNet::RakNetGUID& guid) const noexcept;

    void disconnectClient(const RakNet::RakNetGUID& guid) noexcept;

private:
    struct RakPeerDeleter {
        void operator()(RakNet::RakPeerInterface* peer) const noexcept;
    };

    using SessionsMap = detail::ThreadSafeParallelFlatHashMap<RakNet::RakNetGUID, std::shared_ptr<Session>>;

private:
    void receiveLoop(std::stop_token stopToken);

    void flushLoop(std::stop_token stopToken);

    void processIncomingPacket(RakNet::Packet* packet);

    [[nodiscard]] std::shared_ptr<Session> getSessionShared(const RakNet::RakNetGUID& guid) const noexcept;

    void upsertSession(const RakNet::RakNetGUID& key, std::shared_ptr<Session> session);

    std::shared_ptr<Session> removeSession(const RakNet::RakNetGUID& key);

private:
    std::uint32_t                                             mMaxConnections{};
    std::uint16_t                                             mIpv4Port{};
    std::uint16_t                                             mIpv6Port{};
    std::string                                               mMotd{};
    std::unique_ptr<RakNet::RakPeerInterface, RakPeerDeleter> mPeer{};
    std::unique_ptr<thread::ThreadPool>                       mOwnedThreadPool{};
    thread::ThreadPool*                                       mThreadPool{};
    std::atomic_bool                                          mRunning{false};
    std::jthread                                              mReceiveThread{};
    std::jthread                                              mFlushThread{};
    SessionsMap                                               mSessions{};
    ConnectionEventCallback                                   mOnConnected{};
    ConnectionEventCallback                                   mOnDisconnected{};
    PacketReceiveCallback                                     mOnPacketReceive{};
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
