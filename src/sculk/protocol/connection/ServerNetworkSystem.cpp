// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/ServerNetworkSystem.hpp"
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <latch>
#include <thread>

namespace sculk::protocol::inline abi_v975 {

namespace {

constexpr auto         RECEIVE_IDLE_SLEEP            = std::chrono::milliseconds(1);
constexpr auto         SEND_FLUSH_INTERVAL           = std::chrono::milliseconds(20);
constexpr std::uint8_t MINECRAFT_BATCH_PACKET_ID     = 0xFE;
constexpr std::size_t  MAX_POOLED_PACKET_CAPACITY    = 1U << 20;
constexpr std::size_t  MIN_PARALLEL_PREPARE_SESSIONS = 4;

struct PreparedSend {
    PacketBuffer  mPayload{};
    std::uint32_t mForceReceiptNumber{};
};

class PacketBufferPool final {
public:
    [[nodiscard]] PacketBuffer acquire(std::size_t minCapacity = 0) {
        PacketBuffer buffer;
        if (!mPool.try_dequeue(buffer)) {
            if (minCapacity > 0) {
                buffer.reserve(minCapacity);
            }
            return buffer;
        }

        buffer.clear();
        if (buffer.capacity() < minCapacity) {
            buffer.reserve(minCapacity);
        }
        return buffer;
    }

    void release(PacketBuffer&& buffer) {
        if (buffer.capacity() > MAX_POOLED_PACKET_CAPACITY) {
            return;
        }

        buffer.clear();
        (void)mPool.enqueue(std::move(buffer));
    }

private:
    moodycamel::ConcurrentQueue<PacketBuffer> mPool{};
};

PacketBufferPool gPacketBufferPool{};

[[nodiscard]] PacketBuffer prependMinecraftBatchHeader(std::span<const std::byte> payload) {
    PacketBuffer framed = gPacketBufferPool.acquire(payload.size() + 1);
    framed.push_back(static_cast<std::byte>(MINECRAFT_BATCH_PACKET_ID));
    framed.insert(framed.end(), payload.begin(), payload.end());
    return framed;
}

void prepareBatchedSendsForSession(const std::shared_ptr<Session>& session, PreparedSend& outPrepared) {
    outPrepared.mPayload.clear();
    outPrepared.mForceReceiptNumber = 0;
    if (!session) {
        return;
    }

    thread_local OutboundBatch     outboundBatch;
    thread_local PacketBufferBatch payloadBatch;
    outboundBatch.clear();
    payloadBatch.clear();

    if (session->tryDequeueAllOutboundPackets(outboundBatch) == 0) {
        return;
    }

    payloadBatch.reserve(outboundBatch.size());

    for (auto& outbound : outboundBatch) {
        payloadBatch.push_back(std::move(outbound.mPayload));
    }

    if (!payloadBatch.empty()) {
        auto batched = Session::serializeBatchedPackets(payloadBatch);
        if (!batched.empty()) {
            outPrepared.mPayload = std::move(batched);
        }
    }
}

} // namespace

ServerNetworkSystem::ServerNetworkSystem(std::size_t workerThreadCount)
: mPeer(RakNet::RakPeerInterface::GetInstance()),
  mThreadPool(workerThreadCount),
  mScheduler(mThreadPool) {}

ServerNetworkSystem::~ServerNetworkSystem() { stop(); }

bool ServerNetworkSystem::start(std::uint16_t ipv4Port, std::uint32_t maxConnections) {
    if (mRunning.exchange(true, std::memory_order_acq_rel)) {
        return true;
    }

    RakNet::SocketDescriptor socketDescriptor{ipv4Port, nullptr};
    socketDescriptor.socketFamily = AF_INET;

    const auto startupResult = mPeer->Startup(maxConnections, &socketDescriptor, 1);
    if (startupResult != RakNet::RAKNET_STARTED) {
        mRunning.store(false, std::memory_order_release);
        return false;
    }

    mPeer->SetMaximumIncomingConnections(static_cast<std::uint16_t>(maxConnections));
    mIoThread = std::jthread([this](std::stop_token token) { ioLoop(token); });
    return true;
}

bool ServerNetworkSystem::start(std::uint16_t ipv4Port, std::uint16_t ipv6Port, std::uint32_t maxConnections) {
    if (mRunning.exchange(true, std::memory_order_acq_rel)) {
        return true;
    }

    std::array<RakNet::SocketDescriptor, 2> socketDescriptors{
        RakNet::SocketDescriptor{ipv4Port, nullptr},
        RakNet::SocketDescriptor{ipv6Port, nullptr}
    };
    socketDescriptors[0].socketFamily = AF_INET;
    socketDescriptors[1].socketFamily = AF_INET6;

    const auto startupResult =
        mPeer->Startup(maxConnections, socketDescriptors.data(), static_cast<unsigned>(socketDescriptors.size()));
    if (startupResult != RakNet::RAKNET_STARTED) {
        mRunning.store(false, std::memory_order_release);
        return false;
    }

    mPeer->SetMaximumIncomingConnections(static_cast<std::uint16_t>(maxConnections));
    mIoThread = std::jthread([this](std::stop_token token) { ioLoop(token); });
    return true;
}

void ServerNetworkSystem::stop() {
    if (!mRunning.exchange(false, std::memory_order_acq_rel)) {
        return;
    }

    if (mIoThread.joinable()) {
        mIoThread.request_stop();
        mSendWakeRequested.store(true, std::memory_order_release);
        mIoWaitCv.notify_all();
        mIoThread.join();
    }

    auto oldSessions = mSessionsSnapshot.exchange(
        std::shared_ptr<const SessionMap>{std::make_shared<SessionMap>()},
        std::memory_order_acq_rel
    );
    for (const auto& [_, session] : *oldSessions) {
        if (session) {
            session->markDisconnected();
        }
    }

    if (mPeer) {
        mPeer->Shutdown(0);
    }
}

bool ServerNetworkSystem::isRunning() const noexcept { return mRunning.load(std::memory_order_acquire); }

bool ServerNetworkSystem::sendToClient(RakNet::RakNetGUID guid, std::span<const std::byte> buffer) noexcept {
    auto session = getSession(guid);
    if (!session || !session->sendPacket(buffer)) {
        return false;
    }

    if (session->tryMarkOutboundDirty()) {
        (void)mDirtySessions.enqueue(session);
    }

    bool expectedWake = false;
    if (mSendWakeRequested.compare_exchange_strong(expectedWake, true, std::memory_order_acq_rel)) {
        mIoWaitCv.notify_one();
    }
    return true;
}

std::uint32_t
ServerNetworkSystem::sendToClientImmediately(RakNet::RakNetGUID guid, std::span<const std::byte> buffer) noexcept {
    auto session = getSession(guid);
    if (!session || buffer.empty()) {
        return 0;
    }

    thread_local PacketBufferBatch singlePacketBatch;
    singlePacketBatch.clear();
    singlePacketBatch.emplace_back(buffer.begin(), buffer.end());

    auto batched = Session::serializeBatchedPackets(singlePacketBatch);
    if (batched.empty()) {
        return 0;
    }

    auto receipt = mNextImmediateReceipt.fetch_add(1, std::memory_order_relaxed);
    if (receipt == 0) {
        receipt = mNextImmediateReceipt.fetch_add(1, std::memory_order_relaxed);
    }

    auto framed = prependMinecraftBatchHeader(batched);
    gPacketBufferPool.release(std::move(batched));
    if (!mImmediateSends.enqueue(ImmediateSendRequest{session, std::move(framed), receipt})) {
        gPacketBufferPool.release(std::move(framed));
        return 0;
    }

    bool expectedWake = false;
    if (mSendWakeRequested.compare_exchange_strong(expectedWake, true, std::memory_order_acq_rel)) {
        mIoWaitCv.notify_one();
    }

    return receipt;
}

bool ServerNetworkSystem::receiveFromClient(RakNet::RakNetGUID guid, std::vector<std::byte>& outBuffer) noexcept {
    auto session = getSession(guid);
    return session ? session->receivePacket(outBuffer) : false;
}

bool ServerNetworkSystem::getClientNetworkStatus(RakNet::RakNetGUID guid, NetworkStatus& outStatus) const noexcept {
    auto session = getSession(guid);
    if (!session) {
        return false;
    }

    outStatus = session->getNetworkStatus();
    return true;
}

std::uint64_t ServerNetworkSystem::subscribeEvents(std::function<void(const NetworkEvent&)> handler) {
    if (!handler) {
        return 0;
    }

    const std::uint64_t id            = mNextSubscriptionId.fetch_add(1, std::memory_order_relaxed);
    auto                stableHandler = std::move(handler);

    for (;;) {
        auto current = mEventHandlersSnapshot.load(std::memory_order_acquire);
        auto next    = std::make_shared<EventHandlerVec>(*current);
        next->emplace_back(id, stableHandler);

        auto desired = std::const_pointer_cast<const EventHandlerVec>(next);
        if (mEventHandlersSnapshot
                .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
            return id;
        }
    }

    return id;
}

bool ServerNetworkSystem::unsubscribeEvents(std::uint64_t subscriptionId) {
    if (subscriptionId == 0) {
        return false;
    }

    for (;;) {
        auto current = mEventHandlersSnapshot.load(std::memory_order_acquire);
        auto next    = std::make_shared<EventHandlerVec>();
        next->reserve(current->size());

        bool removed = false;
        for (const auto& entry : *current) {
            if (entry.first == subscriptionId) {
                removed = true;
                continue;
            }
            next->push_back(entry);
        }

        if (!removed) {
            return false;
        }

        auto desired = std::const_pointer_cast<const EventHandlerVec>(next);
        if (mEventHandlersSnapshot
                .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
            return true;
        }
    }
}

std::size_t ServerNetworkSystem::sessionCount() const {
    auto sessions = mSessionsSnapshot.load(std::memory_order_acquire);
    return sessions->size();
}

ServerNetworkSystem::SessionPtr ServerNetworkSystem::getSession(RakNet::RakNetGUID guid) const noexcept {
    auto sessions = mSessionsSnapshot.load(std::memory_order_acquire);
    auto it       = sessions->find(guid.g);
    if (it == sessions->end() || !it->second) {
        return nullptr;
    }

    return it->second;
}

void ServerNetworkSystem::ioLoop(std::stop_token stopToken) {
    auto lastFlush = std::chrono::steady_clock::now() - SEND_FLUSH_INTERVAL;

    while (!stopToken.stop_requested() && mRunning.load(std::memory_order_acquire)) {
        bool progressed = false;

        for (RakNet::Packet* packet = mPeer->Receive(); packet != nullptr; packet = mPeer->Receive()) {
            progressed = true;
            processIncomingPacket(packet);
            mPeer->DeallocatePacket(packet);
        }

        const auto now = std::chrono::steady_clock::now();
        if (now - lastFlush >= SEND_FLUSH_INTERVAL) {
            flushOutboundPackets();
            lastFlush = now;
        }

        if (!progressed) {
            const auto nowIdle    = std::chrono::steady_clock::now();
            const auto untilFlush = (lastFlush + SEND_FLUSH_INTERVAL <= nowIdle)
                                      ? std::chrono::steady_clock::duration::zero()
                                      : (lastFlush + SEND_FLUSH_INTERVAL - nowIdle);
            const auto receiveBudget =
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(RECEIVE_IDLE_SLEEP);
            const auto waitBudget = std::min(untilFlush, receiveBudget);

            std::unique_lock lock(mIoWaitMutex);
            (void)mIoWaitCv.wait_for(lock, waitBudget, [this, &stopToken] {
                if (stopToken.stop_requested()) {
                    return true;
                }

                return mSendWakeRequested.exchange(false, std::memory_order_acq_rel);
            });
        }
    }
}

void ServerNetworkSystem::processIncomingPacket(RakNet::Packet* packet) {
    if (!packet || !packet->data || packet->length == 0) {
        return;
    }

    const auto messageId = packet->data[0];
    const auto key       = packet->guid.g;

    if (messageId == ToMessageID(ID_NEW_INCOMING_CONNECTION)) {
        auto remote          = RakNet::AddressOrGUID{packet->guid};
        remote.systemAddress = packet->systemAddress;
        auto stableSession   = std::make_shared<Session>(mPeer.get(), remote, &mScheduler);

        for (;;) {
            auto current = mSessionsSnapshot.load(std::memory_order_acquire);
            auto next    = std::make_shared<SessionMap>(*current);
            (*next)[key] = stableSession;

            auto desired = std::const_pointer_cast<const SessionMap>(next);
            if (mSessionsSnapshot
                    .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
                break;
            }
        }

        emitEvent(NetworkEvent{NetworkEventType::Connected, packet->guid, packet->systemAddress});
        return;
    }

    if (messageId == ToMessageID(ID_DISCONNECTION_NOTIFICATION) || messageId == ToMessageID(ID_CONNECTION_LOST)) {
        SessionPtr removedSession;
        bool       removed = false;
        for (;;) {
            auto current = mSessionsSnapshot.load(std::memory_order_acquire);
            auto it      = current->find(key);
            if (it == current->end()) {
                break;
            }

            auto next = std::make_shared<SessionMap>(*current);
            auto jt   = next->find(key);
            if (jt != next->end()) {
                removedSession = jt->second;
                next->erase(jt);
            }

            auto desired = std::const_pointer_cast<const SessionMap>(next);
            if (mSessionsSnapshot
                    .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
                removed = static_cast<bool>(removedSession);
                break;
            }
        }

        if (removedSession) {
            removedSession->markDisconnected();
        }

        if (removed) {
            emitEvent(NetworkEvent{NetworkEventType::Disconnected, packet->guid, packet->systemAddress});
        }
        return;
    }

    if (messageId != MINECRAFT_BATCH_PACKET_ID || packet->length <= 1) {
        return;
    }

    auto       sessions = mSessionsSnapshot.load(std::memory_order_acquire);
    auto       it       = sessions->find(key);
    SessionPtr session  = (it != sessions->end()) ? it->second : nullptr;

    if (!session) {
        return;
    }

    const auto* payloadBegin = reinterpret_cast<const std::byte*>(packet->data + 1);
    const auto  payloadSize  = static_cast<std::size_t>(packet->length - 1);
    auto        packets      = Session::deserializeBatchPackets(std::span<const std::byte>{payloadBegin, payloadSize});
    for (auto& payload : packets) {
        (void)session->enqueueInboundPacket(std::move(payload));
    }
}

void ServerNetworkSystem::emitEvent(NetworkEvent event) {
    auto handlers = mEventHandlersSnapshot.load(std::memory_order_acquire);
    if (handlers->empty()) {
        return;
    }

    for (const auto& [_, handler] : *handlers) {
        if (!mThreadPool.submit([event, handler]() mutable { handler(event); })) {
            handler(event);
        }
    }
}

void ServerNetworkSystem::flushOutboundPackets() {
    ImmediateSendRequest immediate;
    while (mImmediateSends.try_dequeue(immediate)) {
        if (immediate.mSession && immediate.mSession->isConnected()) {
            (void)immediate.mSession->sendPacketImmediately(immediate.mPayload, immediate.mForceReceiptNumber);
        }
        gPacketBufferPool.release(std::move(immediate.mPayload));
    }

    thread_local std::vector<SessionPtr>             sessions;
    thread_local phmap::flat_hash_set<std::uint64_t> seenSessionGuids;
    sessions.clear();
    seenSessionGuids.clear();
    seenSessionGuids.reserve(64);

    SessionPtr dirtySession;
    while (mDirtySessions.try_dequeue(dirtySession)) {
        if (!dirtySession || !dirtySession->isConnected()) {
            continue;
        }

        const auto guidValue = dirtySession->guid().g;
        if (!seenSessionGuids.insert(guidValue).second) {
            continue;
        }

        dirtySession->clearOutboundDirty();
        sessions.push_back(std::move(dirtySession));
    }

    if (sessions.empty()) {
        return;
    }

    thread_local std::vector<PreparedSend> preparedBySession;
    preparedBySession.clear();
    preparedBySession.resize(sessions.size());

    const bool shouldParallelPrepare =
        sessions.size() >= MIN_PARALLEL_PREPARE_SESSIONS && mThreadPool.threadCount() > 1;

    if (shouldParallelPrepare) {
        auto*      preparedBySessionPtr = &preparedBySession;
        std::latch phase1Done(static_cast<std::ptrdiff_t>(sessions.size()));

        for (std::size_t i = 0; i < sessions.size(); ++i) {
            auto task = [session = sessions[i], i, preparedBySessionPtr, &phase1Done]() mutable {
                prepareBatchedSendsForSession(session, (*preparedBySessionPtr)[i]);
                phase1Done.count_down();
            };

            if (!mThreadPool.submit(task)) {
                task();
            }
        }

        phase1Done.wait();
    } else {
        for (std::size_t i = 0; i < sessions.size(); ++i) {
            prepareBatchedSendsForSession(sessions[i], preparedBySession[i]);
        }
    }

    // Keep RakPeer send calls on the I/O thread for transport safety.
    for (std::size_t i = 0; i < sessions.size(); ++i) {
        auto& prepared = preparedBySession[i];
        if (!prepared.mPayload.empty()) {
            auto framed = prependMinecraftBatchHeader(prepared.mPayload);
            (void)sessions[i]->sendPacketImmediately(framed, prepared.mForceReceiptNumber);
            gPacketBufferPool.release(std::move(prepared.mPayload));
            gPacketBufferPool.release(std::move(framed));
        }

        if (sessions[i]->hasPendingOutboundPackets() && sessions[i]->tryMarkOutboundDirty()) {
            (void)mDirtySessions.enqueue(sessions[i]);
        }
    }
}

void ServerNetworkSystem::RakPeerDeleter::operator()(RakNet::RakPeerInterface* peer) const noexcept {
    if (peer) {
        RakNet::RakPeerInterface::DestroyInstance(peer);
    }
}

} // namespace sculk::protocol::inline abi_v975
