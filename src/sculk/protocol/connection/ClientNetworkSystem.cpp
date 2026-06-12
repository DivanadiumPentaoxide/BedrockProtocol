// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/ClientNetworkSystem.hpp"
#include "sculk/protocol/codec/MinecraftPackets.hpp"
#include <MessageIdentifiers.h>
#include <chrono>
#include <thread>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace {

constexpr std::uint8_t MINECRAFT_BATCH_PACKET_ID = 0xFE;
constexpr auto         RECEIVE_TICK_INTERVAL     = std::chrono::milliseconds(1);
constexpr auto         FLUSH_TICK_INTERVAL       = std::chrono::milliseconds(1);

} // namespace

ClientNetworkSystem::ClientNetworkSystem(std::size_t workerThreadCount)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(std::make_unique<thread::ThreadPool>(workerThreadCount)),
  mThreadPool(mOwnedThreadPool.get()),
  mSession(std::shared_ptr<Session>{}) {}

ClientNetworkSystem::ClientNetworkSystem(thread::ThreadPool& threadPool)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mOwnedThreadPool(nullptr),
  mThreadPool(&threadPool),
  mSession(std::shared_ptr<Session>{}) {}

ClientNetworkSystem::ClientNetworkSystem(thread::ThreadPool& threadPool, io::ClientIoRuntime&)
: ClientNetworkSystem(threadPool) {}

ClientNetworkSystem::~ClientNetworkSystem() { disconnect(); }

bool ClientNetworkSystem::connect(std::string_view host, std::uint16_t port) {
    if (mRunning.exchange(true, std::memory_order_acq_rel)) {
        return false;
    }

    std::array<RakNet::SocketDescriptor, 2> descriptor{};
    descriptor[0].socketFamily = AF_INET;
    descriptor[1].socketFamily = AF_INET6;

    const auto startupResult = mPeer->Startup(1, descriptor.data(), 2);
    if (startupResult != RakNet::RAKNET_STARTED) {
        mRunning.store(false, std::memory_order_release);
        return false;
    }

    const auto connectResult = mPeer->Connect(host.data(), port, nullptr, 0);
    if (connectResult != RakNet::CONNECTION_ATTEMPT_STARTED) {
        mPeer->Shutdown(0);
        mRunning.store(false, std::memory_order_release);
        return false;
    }

    mReceiveThread = std::jthread([this](std::stop_token token) { receiveLoop(token); });
    mFlushThread   = std::jthread([this](std::stop_token token) { flushLoop(token); });
    return true;
}

void ClientNetworkSystem::disconnect() {
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

    // Keep the session object alive so delayed callbacks that call getSession()
    // do not dereference a null pointer after disconnect.
    auto session = mSession.load(std::memory_order_acquire);
    if (session) {
        session->disconnect();
    }

    if (mPeer) {
        mPeer->Shutdown(20);
    }
}

bool ClientNetworkSystem::isRunning() const noexcept { return mRunning.load(std::memory_order_acquire); }

bool ClientNetworkSystem::isConnected() const noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    return session && session->isConnected();
}

bool ClientNetworkSystem::getServerNetworkStatus(NetworkStatus& outStatus) const noexcept {
    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return false;
    }

    outStatus = session->getNetworkStatus();
    return true;
}

bool ClientNetworkSystem::setOnConnected(ConnectionEventCallback&& callback) noexcept {
    if (mRunning.load(std::memory_order_acquire)) {
        return false;
    }
    mOnConnected = std::move(callback);
    return true;
}

bool ClientNetworkSystem::setOnDisconnected(ConnectionEventCallback&& callback) noexcept {
    if (mRunning.load(std::memory_order_acquire)) {
        return false;
    }
    mOnDisconnected = std::move(callback);
    return true;
}

bool ClientNetworkSystem::setOnConnectionFailed(ConnectionEventCallback&& callback) noexcept {
    if (mRunning.load(std::memory_order_acquire)) {
        return false;
    }
    mOnConnectionFailed = std::move(callback);
    return true;
}

bool ClientNetworkSystem::setOnPacketReceive(PacketReceiveCallback&& callback) {
    if (mRunning.load(std::memory_order_acquire)) {
        return false;
    }
    mOnPacketReceive = std::move(callback);
    return true;
}

Session& ClientNetworkSystem::getSession() const noexcept { return *mSession.load(std::memory_order_acquire); }

bool ClientNetworkSystem::getNetworkStatus(NetworkStatus& outStatus) const noexcept {
    return getServerNetworkStatus(outStatus);
}

bool ClientNetworkSystem::ioTickOnce() noexcept {
    bool progressed = false;

    while (RakNet::Packet* packet = mPeer->Receive()) {
        processIncomingPacket(packet);
        mPeer->DeallocatePacket(packet);
        progressed = true;
    }

    auto session = mSession.load(std::memory_order_acquire);
    if (session) {
        progressed = session->flushIfDue(std::chrono::steady_clock::now()) || progressed;
    }

    return progressed;
}

void ClientNetworkSystem::receiveLoop(std::stop_token stopToken) {
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

void ClientNetworkSystem::flushLoop(std::stop_token stopToken) {
    while (!stopToken.stop_requested() && mRunning.load(std::memory_order_acquire)) {
        const auto tickBegin = std::chrono::steady_clock::now();

        auto session = mSession.load(std::memory_order_acquire);
        if (session && session->isConnected()) {
            (void)session->flushIfDue(std::chrono::steady_clock::now());
        }

        const auto nextTick = tickBegin + FLUSH_TICK_INTERVAL;
        const auto now      = std::chrono::steady_clock::now();
        if (now < nextTick) {
            std::this_thread::sleep_until(nextTick);
        }
    }
}

void ClientNetworkSystem::processIncomingPacket(RakNet::Packet* packet) {
    if (!packet || !packet->data || packet->length == 0) {
        return;
    }

    const auto messageId = packet->data[0];

    if (messageId == DefaultMessageIDTypes::ID_CONNECTION_REQUEST_ACCEPTED) {
        auto remote  = RakNet::AddressOrGUID{packet};
        auto session = std::make_shared<Session>(mPeer.get(), remote);
        mSession.store(session, std::memory_order_release);
        if (mOnConnected) {
            auto onConnected = mOnConnected;
            (void)mThreadPool->submit([onConnected = std::move(onConnected)]() mutable noexcept {
                if (onConnected) {
                    onConnected();
                }
            });
        }
        return;
    }

    if (messageId == DefaultMessageIDTypes::ID_DISCONNECTION_NOTIFICATION
        || messageId == DefaultMessageIDTypes::ID_CONNECTION_LOST) {

        auto session = mSession.load(std::memory_order_acquire);
        if (session) {
            session->disconnect();
        }

        if (mOnDisconnected) {
            auto onDisconnected = mOnDisconnected;
            (void)mThreadPool->submit([onDisconnected = std::move(onDisconnected)]() mutable noexcept {
                if (onDisconnected) {
                    onDisconnected();
                }
            });
        }
        mRunning.store(false, std::memory_order_release);
        return;
    }

    if (messageId == DefaultMessageIDTypes::ID_CONNECTION_ATTEMPT_FAILED
        || messageId == DefaultMessageIDTypes::ID_NO_FREE_INCOMING_CONNECTIONS) {

        auto session = mSession.load(std::memory_order_acquire);
        if (session) {
            session->disconnect();
        }

        if (mOnConnectionFailed) {
            auto onConnectionFailed = mOnConnectionFailed;
            (void)mThreadPool->submit([onConnectionFailed = std::move(onConnectionFailed)]() mutable noexcept {
                if (onConnectionFailed) {
                    onConnectionFailed();
                }
            });
        }
        mRunning.store(false, std::memory_order_release);
        return;
    }

    if (messageId != MINECRAFT_BATCH_PACKET_ID || packet->length <= 1) {
        return;
    }

    auto session = mSession.load(std::memory_order_acquire);
    if (!session) {
        return;
    }

    const auto* payloadBegin = reinterpret_cast<const std::byte*>(packet->data + 1);
    const auto  payloadSize  = static_cast<std::size_t>(packet->length - 1);

    auto packets = session->deserializeBatchPackets(std::span<const std::byte>{payloadBegin, payloadSize});
    if (!packets) {
        return;
    }

    auto onPacketReceive = mOnPacketReceive;
    for (auto& payload : *packets) {
        if (onPacketReceive) {
            auto packetObj = MinecraftPackets::readAndCreatePacketFromBuffer(payload);
            (void)mThreadPool->submit([onPacketReceive, packet = std::move(packetObj)]() mutable noexcept {
                if (onPacketReceive) {
                    onPacketReceive(std::move(packet));
                }
            });
        } else {
            (void)session->enqueueInboundPacket(std::move(payload));
        }
    }
}

void ClientNetworkSystem::RakPeerDeleter::operator()(RakNet::RakPeerInterface* peer) const noexcept {
    if (peer) {
        RakNet::RakPeerInterface::DestroyInstance(peer);
    }
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
