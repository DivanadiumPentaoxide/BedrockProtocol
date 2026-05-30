// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/Session.hpp"
#include "sculk/protocol/connection/coro/Scheduler.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include <PacketPriority.h>
#include <RakNetStatistics.h>
#include <chrono>

namespace sculk::protocol::inline abi_v975 {

namespace {

[[nodiscard]] NetworkStatus::ConnectionState mapConnectionState(RakNet::ConnectionState state) noexcept {
    switch (state) {
    case RakNet::IS_CONNECTED:
        return NetworkStatus::ConnectionState::Connected;
    case RakNet::IS_CONNECTING:
        return NetworkStatus::ConnectionState::Connecting;
    case RakNet::IS_DISCONNECTING:
        return NetworkStatus::ConnectionState::Disconnecting;
    case RakNet::IS_PENDING:
    case RakNet::IS_SILENTLY_DISCONNECTING:
    case RakNet::IS_DISCONNECTED:
    case RakNet::IS_NOT_CONNECTED:
        return NetworkStatus::ConnectionState::Disconnected;
    default:
        return NetworkStatus::ConnectionState::Unknown;
    }
}

class ReceiveNotificationAwaitable final {
public:
    explicit ReceiveNotificationAwaitable(Session& session) noexcept : mSession(session) {}

    [[nodiscard]] bool await_ready() const noexcept { return mSession.hasPendingInboundPackets(); }

    bool await_suspend(std::coroutine_handle<> handle) noexcept {
        mSession.registerReceiveWaiter(handle);
        if (mSession.hasPendingInboundPackets()) {
            mSession.notifyOneReceiver();
        }
        return true;
    }

    void await_resume() const noexcept {}

private:
    Session& mSession;
};

} // namespace

Session::Session(RakNet::RakPeerInterface* peer, RakNet::AddressOrGUID remote, coro::Scheduler* scheduler) noexcept
: mPeer(peer),
  mRemote(remote),
  mScheduler(scheduler) {}

bool Session::sendPacket(std::span<const std::byte> buffer) noexcept {
    if (!mConnected.load(std::memory_order_relaxed)) {
        return false;
    }

    OutboundPacket packet{};
    packet.mPayload.assign(buffer.begin(), buffer.end());
    return mOutboundPackets.enqueue(std::move(packet));
}

bool Session::sendPacket(std::vector<std::byte>&& buffer) noexcept {
    if (!mConnected.load(std::memory_order_relaxed)) {
        return false;
    }

    OutboundPacket packet{};
    packet.mPayload = std::move(buffer);
    return mOutboundPackets.enqueue(std::move(packet));
}

std::uint32_t
Session::sendPacketImmediately(std::span<const std::byte> buffer, std::uint32_t forceReceiptNumber) noexcept {
    if (!mConnected.load(std::memory_order_relaxed) || !mPeer || buffer.empty()) {
        return 0;
    }

    return mPeer->Send(
        reinterpret_cast<const char*>(buffer.data()),
        static_cast<int>(buffer.size()),
        HIGH_PRIORITY,
        RELIABLE_ORDERED,
        0,
        mRemote,
        false,
        forceReceiptNumber
    );
}

bool Session::receivePacket(std::vector<std::byte>& outBuffer) noexcept {
    return mInboundPackets.try_dequeue(outBuffer);
}

coro::Task<Result<std::vector<std::byte>>> Session::receivePacketAsync() {
    std::vector<std::byte> output;
    for (;;) {
        if (receivePacket(output)) {
            co_return output;
        }

        if (!isConnected()) {
            co_return error_utils::makeError("session disconnected");
        }

        co_await ReceiveNotificationAwaitable{*this};
    }
}

bool Session::hasPendingInboundPackets() const noexcept { return mInboundPackets.size_approx() > 0; }

bool Session::hasPendingOutboundPackets() const noexcept { return mOutboundPackets.size_approx() > 0; }

bool Session::tryMarkOutboundDirty() noexcept {
    bool expected = false;
    return mOutboundDirty.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
}

void Session::clearOutboundDirty() noexcept { mOutboundDirty.store(false, std::memory_order_release); }

bool Session::isConnected() const noexcept { return mConnected.load(std::memory_order_relaxed); }

RakNet::RakNetGUID Session::guid() const noexcept { return mRemote.rakNetGuid; }

RakNet::AddressOrGUID Session::remoteEndpoint() const noexcept { return mRemote; }

NetworkStatus Session::getNetworkStatus() const noexcept {
    NetworkStatus status{};
    status.mGuid        = mRemote.rakNetGuid;
    status.mSampledAtMs = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count()
    );
    status.mConnected           = mConnected.load(std::memory_order_relaxed);
    status.mInboundQueueApprox  = static_cast<std::size_t>(mInboundPackets.size_approx());
    status.mOutboundQueueApprox = static_cast<std::size_t>(mOutboundPackets.size_approx());

    if (!mPeer || mRemote.IsUndefined()) {
        status.mConnectionState =
            status.mConnected ? NetworkStatus::ConnectionState::Unknown : NetworkStatus::ConnectionState::Disconnected;
        return status;
    }

    auto resolvedRemote = mRemote;
    if (resolvedRemote.systemAddress == RakNet::UNASSIGNED_SYSTEM_ADDRESS
        && resolvedRemote.rakNetGuid != RakNet::UNASSIGNED_RAKNET_GUID) {
        resolvedRemote.systemAddress = mPeer->GetSystemAddressFromGuid(resolvedRemote.rakNetGuid);
    }

    const auto peerState    = mPeer->GetConnectionState(resolvedRemote);
    status.mConnectionState = mapConnectionState(peerState);
    status.mConnected       = status.mConnected && (peerState == RakNet::IS_CONNECTED);

    status.mAveragePingMs = mPeer->GetAveragePing(resolvedRemote);
    status.mLastPingMs    = mPeer->GetLastPing(resolvedRemote);
    status.mLowestPingMs  = mPeer->GetLowestPing(resolvedRemote);

    RakNet::RakNetStatistics transportStats{};
    auto*                    stats = mPeer->GetStatistics(resolvedRemote.systemAddress, &transportStats);
    if (!stats) {
        return status;
    }

    status.mHasTransportStatistics = true;
    status.mPacketLossLastSecond   = stats->packetlossLastSecond;
    status.mPacketLossTotal        = stats->packetlossTotal;

    status.mUserMessageBytesPushedPerSecond =
        stats->valueOverLastSecond[static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::USER_MESSAGE_BYTES_PUSHED)];
    status.mUserMessageBytesSentPerSecond =
        stats->valueOverLastSecond[static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::USER_MESSAGE_BYTES_SENT)];
    status.mUserMessageBytesResentPerSecond =
        stats->valueOverLastSecond[static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::USER_MESSAGE_BYTES_RESENT)];
    status.mUserMessageBytesReceivedPerSecond = stats->valueOverLastSecond[static_cast<std::uint32_t>(
        RakNet::RNSPerSecondMetrics::USER_MESSAGE_BYTES_RECEIVED_PROCESSED
    )];
    status.mActualBytesSentPerSecond =
        stats->valueOverLastSecond[static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::ACTUAL_BYTES_SENT)];
    status.mActualBytesReceivedPerSecond =
        stats->valueOverLastSecond[static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::ACTUAL_BYTES_RECEIVED)];

    status.mUserMessageBytesSentTotal =
        stats->runningTotal[static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::USER_MESSAGE_BYTES_SENT)];
    status.mUserMessageBytesReceivedTotal = stats->runningTotal[static_cast<std::uint32_t>(
        RakNet::RNSPerSecondMetrics::USER_MESSAGE_BYTES_RECEIVED_PROCESSED
    )];
    status.mActualBytesSentTotal =
        stats->runningTotal[static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::ACTUAL_BYTES_SENT)];
    status.mActualBytesReceivedTotal =
        stats->runningTotal[static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::ACTUAL_BYTES_RECEIVED)];

    status.mBytesInResendBuffer    = stats->bytesInResendBuffer;
    status.mMessagesInResendBuffer = stats->messagesInResendBuffer;

    status.mBytesQueuedForSend = 0;
    for (std::size_t i = 0; i < static_cast<std::size_t>(PacketPriority::NUMBER_OF_PRIORITIES); ++i) {
        status.mBytesQueuedForSend += static_cast<std::uint64_t>(stats->bytesInSendBuffer[i]);
    }

    status.mLimitedByCongestionControl      = stats->isLimitedByCongestionControl;
    status.mLimitedByOutgoingBandwidthLimit = stats->isLimitedByOutgoingBandwidthLimit;
    status.mCongestionControlBpsLimit       = stats->BPSLimitByCongestionControl;
    status.mOutgoingBandwidthBpsLimit       = stats->BPSLimitByOutgoingBandwidthLimit;

    return status;
}

void Session::markDisconnected() noexcept {
    mConnected.store(false, std::memory_order_relaxed);

    std::coroutine_handle<> handle;
    while (mReceiveWaiters.try_dequeue(handle)) {
        if (mScheduler) {
            if (mScheduler->schedule(handle)) {
                continue;
            }
        }

        if (handle) {
            handle.resume();
        }
    }
}

bool Session::enqueueInboundPacket(std::vector<std::byte>&& buffer) noexcept {
    if (!mConnected.load(std::memory_order_relaxed)) {
        return false;
    }

    const bool ok = mInboundPackets.enqueue(std::move(buffer));
    if (ok) {
        notifyOneReceiver();
    }
    return ok;
}

bool Session::tryDequeueOutboundPacket(OutboundPacket& outPacket) noexcept {
    return mOutboundPackets.try_dequeue(outPacket);
}

std::size_t Session::tryDequeueAllOutboundPackets(OutboundBatch& outPackets) noexcept {
    outPackets.clear();

    OutboundPacket packet;
    while (mOutboundPackets.try_dequeue(packet)) {
        outPackets.push_back(std::move(packet));
    }

    return outPackets.size();
}

PacketBuffer Session::serializeBatchedPackets(const PacketBufferBatch& packets) {
    PacketBuffer buffer{};
    BinaryStream stream{buffer};
    for (const auto& packet : packets) {
        stream.writeUnsignedVarInt(static_cast<std::uint32_t>(packet.size()));
        stream.writeBytes(packet.data(), packet.size());
    }
    // TODO
    return buffer;
}

PacketBufferBatch Session::deserializeBatchPackets(std::span<const std::byte> batchedBuffer) {
    ReadOnlyBinaryStream stream{batchedBuffer};
    PacketBufferBatch    packets{};

    while (stream.hasDataLeft()) {
        std::uint32_t packetSize{};
        if (!stream.readUnsignedVarInt(packetSize)) {
            return {};
        }
        std::vector<std::byte> packetData(packetSize, std::byte{});
        if (!stream.readBytes(packetData.data(), packetSize)) {
            return {};
        }
        packets.push_back(std::move(packetData));
    }
    // TODO

    return packets;
}

void Session::notifyOneReceiver() noexcept {
    std::coroutine_handle<> handle;
    if (!mReceiveWaiters.try_dequeue(handle)) {
        return;
    }

    if (mScheduler) {
        if (mScheduler->schedule(handle)) {
            return;
        }
    }

    if (handle) {
        handle.resume();
    }
}

void Session::registerReceiveWaiter(std::coroutine_handle<> handle) noexcept { mReceiveWaiters.enqueue(handle); }

} // namespace sculk::protocol::inline abi_v975
