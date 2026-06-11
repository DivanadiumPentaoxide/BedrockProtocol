// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/ServerNetworkSystem.hpp"
#include "sculk/protocol/codec/MinecraftPackets.hpp"
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <format>
#include <thread>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace {

constexpr std::uint8_t MINECRAFT_BATCH_PACKET_ID = 0xFE;
constexpr auto         RECEIVE_TICK_INTERVAL     = std::chrono::milliseconds(1);
constexpr auto         FLUSH_TICK_INTERVAL       = std::chrono::milliseconds(1);
constexpr std::size_t  FLUSH_WINDOW_TICKS        = 20;

[[nodiscard]] std::size_t flushBudgetPerTick(std::size_t sessionsCount) noexcept {
    if (sessionsCount == 0) {
        return 0;
    }

    return std::max<std::size_t>(1, (sessionsCount + (FLUSH_WINDOW_TICKS - 1)) / FLUSH_WINDOW_TICKS);
}

} // namespace

ServerNetworkSystem::ServerNetworkSystem(std::size_t workerThreadCount)
: mMotd("Sculk Protocol Library"),
  mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(std::make_unique<thread::ThreadPool>(workerThreadCount)),
  mThreadPool(mOwnedThreadPool.get()),
  mSessionsState(std::make_shared<SessionsState>()) {}

ServerNetworkSystem::ServerNetworkSystem(thread::ThreadPool& threadPool)
: mMotd("Sculk Protocol Library"),
  mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(nullptr),
  mThreadPool(&threadPool),
  mSessionsState(std::make_shared<SessionsState>()) {}

ServerNetworkSystem::~ServerNetworkSystem() { stop(); }

bool ServerNetworkSystem::start(std::uint16_t ipv4Port, std::uint32_t maxConnections) {
    if (mRunning.exchange(true, std::memory_order_acq_rel)) {
        return false;
    }

    mIpv4Port = ipv4Port;
    mIpv6Port.reset();
    mMaxConnections = maxConnections;

    RakNet::SocketDescriptor socketDescriptor{mIpv4Port, nullptr};
    socketDescriptor.socketFamily = AF_INET;

    const auto status = mPeer->Startup(mMaxConnections, &socketDescriptor, 1);
    if (status != RakNet::RAKNET_STARTED) {
        mRunning.store(false, std::memory_order_release);
        return false;
    }

    mPeer->SetMaximumIncomingConnections(mMaxConnections);
    updateAnnouncement();

    mReceiveThread = std::jthread([this](std::stop_token token) { receiveLoop(token); });
    mFlushThread   = std::jthread([this](std::stop_token token) { flushLoop(token); });
    return true;
}

bool ServerNetworkSystem::start(std::uint16_t ipv4Port, std::uint16_t ipv6Port, std::uint32_t maxConnections) {
    if (mRunning.exchange(true, std::memory_order_acq_rel)) {
        return false;
    }

    mIpv4Port       = ipv4Port;
    mIpv6Port       = ipv6Port;
    mMaxConnections = maxConnections;

    std::array<RakNet::SocketDescriptor, 2> descriptors{
        RakNet::SocketDescriptor{mIpv4Port,  nullptr},
        RakNet::SocketDescriptor{*mIpv6Port, nullptr}
    };
    descriptors[0].socketFamily = AF_INET;
    descriptors[1].socketFamily = AF_INET6;

    auto status = mPeer->Startup(mMaxConnections, descriptors.data(), 2);
    if (status != RakNet::RAKNET_STARTED) {
        mRunning.store(false, std::memory_order_release);
        return false;
    }

    mPeer->SetMaximumIncomingConnections(mMaxConnections);
    updateAnnouncement();

    mReceiveThread = std::jthread([this](std::stop_token token) { receiveLoop(token); });
    mFlushThread   = std::jthread([this](std::stop_token token) { flushLoop(token); });
    return true;
}

void ServerNetworkSystem::setMotd(std::string_view motd) {
    mMotd = motd;
    updateAnnouncement();
}

void ServerNetworkSystem::updateAnnouncement() noexcept {
    auto message = std::format(
        "MCPE;{0};{1};{2};{3};{4};{5};Sculk Sensor Proxy Server;Survival;1;{6};{7};0;",
        mMotd,
        getProtocolVersion(),
        getMinecraftVersion(),
        getSessionsCount(),
        mMaxConnections,
        mPeer->GetMyGUID().ToString(),
        mIpv4Port,
        mIpv6Port.value_or(0)
    );
    std::uint16_t length = static_cast<std::uint16_t>(message.size());
    message.insert(0, 2, '\0');
    message[0] = static_cast<char>((length >> 8) & 0xFF);
    message[1] = static_cast<char>(length & 0xFF);

    mPeer->SetOfflinePingResponse(message.data(), static_cast<unsigned int>(message.size()));
}

void ServerNetworkSystem::setMaxConnections(std::uint32_t maxConnections) {
    mMaxConnections = maxConnections;
    mPeer->SetMaximumIncomingConnections(mMaxConnections);
    updateAnnouncement();
}

void ServerNetworkSystem::stop() {
    if (!mRunning.exchange(false, std::memory_order_acq_rel)) {
        return;
    }

    if (mReceiveThread.joinable()) {
        mReceiveThread.request_stop();
        mReceiveThread.join();
    }

    if (mFlushThread.joinable()) {
        mFlushThread.request_stop();
        mFlushThread.join();
    }

    std::shared_ptr<const SessionsState> previousState{};
    previousState = mSessionsState.exchange(std::make_shared<SessionsState>(), std::memory_order_acq_rel);

    for (const auto& session : previousState->mOrdered) {
        if (session) {
            session->disconnect();
        }
    }

    if (mPeer) {
        mPeer->Shutdown(0);
    }
}

bool ServerNetworkSystem::isRunning() const noexcept { return mRunning.load(std::memory_order_acquire); }

bool ServerNetworkSystem::getClientNetworkStatus(
    const RakNet::RakNetGUID& guid,
    NetworkStatus&            outStatus
) const noexcept {
    auto session = getSession(guid);
    if (!session) {
        return false;
    }

    outStatus = session->getNetworkStatus();
    return true;
}

std::size_t ServerNetworkSystem::getSessionsCount() const {
    return mSessionsState.load(std::memory_order_acquire)->mByGuid.size();
}

bool ServerNetworkSystem::setOnConnected(ConnectionEventCallback&& callback) noexcept {
    if (mRunning.load(std::memory_order_acquire)) {
        return false;
    }
    mOnConnected = std::move(callback);
    return true;
}

bool ServerNetworkSystem::setOnDisconnected(ConnectionEventCallback&& callback) noexcept {
    if (mRunning.load(std::memory_order_acquire)) {
        return false;
    }
    mOnDisconnected = std::move(callback);
    return true;
}

bool ServerNetworkSystem::setOnPacketReceive(PacketReceiveCallback&& callback) {
    if (mRunning.load(std::memory_order_acquire)) {
        return false;
    }
    mOnPacketReceive = std::move(callback);
    return true;
}

std::shared_ptr<Session> ServerNetworkSystem::getSession(const RakNet::RakNetGUID& guid) const noexcept {
    const auto sessions = mSessionsState.load(std::memory_order_acquire);
    auto       it       = sessions->mByGuid.find(guid.g);
    if (it == sessions->mByGuid.end()) {
        return nullptr;
    }

    return it->second;
}

void ServerNetworkSystem::disconnectClient(const RakNet::RakNetGUID& guid) noexcept {
    auto session = getSession(guid);
    if (session) {
        session->disconnect();
    }
    removeSession(guid.g);
}

void ServerNetworkSystem::receiveLoop(std::stop_token stopToken) {
    while (!stopToken.stop_requested() && mRunning.load(std::memory_order_acquire)) {
        const auto tickBegin = std::chrono::steady_clock::now();

        while (RakNet::Packet* packet = mPeer->Receive()) {
            processIncomingPacket(packet);
            mPeer->DeallocatePacket(packet);
        }

        const auto nextTick = tickBegin + RECEIVE_TICK_INTERVAL;
        const auto now      = std::chrono::steady_clock::now();
        if (now < nextTick) {
            std::this_thread::sleep_until(nextTick);
        }
    }
}

void ServerNetworkSystem::flushLoop(std::stop_token stopToken) {
    std::size_t cursor = 0;

    while (!stopToken.stop_requested() && mRunning.load(std::memory_order_acquire)) {
        const auto tickBegin = std::chrono::steady_clock::now();
        const auto now       = std::chrono::steady_clock::now();

        const auto sessions = mSessionsState.load(std::memory_order_acquire);
        const auto total    = sessions->mOrdered.size();
        const auto budget   = flushBudgetPerTick(total);

        for (std::size_t i = 0; i < budget; ++i) {
            if (total == 0) {
                break;
            }

            const auto index   = (cursor + i) % total;
            auto&      session = sessions->mOrdered[index];
            if (!session || !session->isConnected()) {
                continue;
            }

            (void)session->flushIfDue(now);
        }

        if (total > 0) {
            cursor = (cursor + budget) % total;
        }

        const auto nextTick = tickBegin + FLUSH_TICK_INTERVAL;
        const auto current  = std::chrono::steady_clock::now();
        if (current < nextTick) {
            std::this_thread::sleep_until(nextTick);
        }
    }
}

void ServerNetworkSystem::processIncomingPacket(RakNet::Packet* packet) {
    if (!packet || !packet->data || packet->length == 0) {
        return;
    }

    const auto messageId = packet->data[0];
    const auto key       = packet->guid.g;

    if (messageId == DefaultMessageIDTypes::ID_UNCONNECTED_PING) {
        return;
    }

    if (messageId == DefaultMessageIDTypes::ID_NEW_INCOMING_CONNECTION) {
        auto remote  = RakNet::AddressOrGUID{packet};
        auto session = std::make_shared<Session>(mPeer.get(), remote);
        upsertSession(key, session);

        if (mOnConnected) {
            (void)mThreadPool->submit([this, guid = packet->guid, address = packet->systemAddress]() mutable noexcept {
                mOnConnected(guid, address);
            });
        }
        return;
    }

    if (messageId == DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION
        || messageId == DefaultMessageIDTypes::ID_CONNECTION_LOST) {

        auto removed = removeSession(key);
        if (removed) {
            removed->disconnect();
        }

        if (mOnDisconnected) {
            (void)mThreadPool->submit([this, guid = packet->guid, address = packet->systemAddress]() mutable noexcept {
                mOnDisconnected(guid, address);
            });
        }
        return;
    }

    if (messageId != MINECRAFT_BATCH_PACKET_ID || packet->length <= 1) {
        return;
    }

    auto session = getSession(packet->guid);
    if (!session) {
        return;
    }

    const auto* payloadBegin = reinterpret_cast<const std::byte*>(packet->data + 1);
    const auto  payloadSize  = static_cast<std::size_t>(packet->length - 1);

    auto packets = session->deserializeBatchPackets(std::span<const std::byte>{payloadBegin, payloadSize});
    if (!packets) {
        return;
    }

    for (auto& payload : *packets) {
        if (mOnPacketReceive) {
            auto packetObj = MinecraftPackets::readAndCreatePacketFromBuffer(payload);
            (void)mThreadPool->submit([this,
                                       guid    = packet->guid,
                                       address = packet->systemAddress,
                                       packet  = std::move(packetObj)]() mutable noexcept {
                mOnPacketReceive(guid, address, std::move(packet));
            });
        } else {
            (void)session->enqueueInboundPacket(std::move(payload));
        }
    }
}

void ServerNetworkSystem::upsertSession(std::uint64_t key, std::shared_ptr<Session> session) {
    auto current = mSessionsState.load(std::memory_order_acquire);
    for (;;) {
        auto next = std::make_shared<SessionsState>(*current);
        next->mByGuid.insert_or_assign(key, session);
        next->mOrdered.clear();
        next->mOrdered.reserve(next->mByGuid.size());
        for (auto& [_, currentSession] : next->mByGuid) {
            next->mOrdered.push_back(currentSession);
        }

        if (mSessionsState.compare_exchange_weak(current, next, std::memory_order_release, std::memory_order_acquire)) {
            break;
        }
    }
}

std::shared_ptr<Session> ServerNetworkSystem::removeSession(std::uint64_t key) {
    auto current = mSessionsState.load(std::memory_order_acquire);
    for (;;) {
        auto it = current->mByGuid.find(key);
        if (it == current->mByGuid.end()) {
            return nullptr;
        }

        auto removed = it->second;
        auto next    = std::make_shared<SessionsState>(*current);
        next->mByGuid.erase(key);
        next->mOrdered.clear();
        next->mOrdered.reserve(next->mByGuid.size());
        for (auto& [_, currentSession] : next->mByGuid) {
            next->mOrdered.push_back(currentSession);
        }

        if (mSessionsState.compare_exchange_weak(current, next, std::memory_order_release, std::memory_order_acquire)) {
            return removed;
        }
    }
}

void ServerNetworkSystem::RakPeerDeleter::operator()(RakNet::RakPeerInterface* peer) const noexcept {
    if (peer) {
        RakNet::RakPeerInterface::DestroyInstance(peer);
    }
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
