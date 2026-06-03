// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/Session.hpp"
#include "sculk/protocol/connection/compression/Snappy.hpp"
#include "sculk/protocol/connection/compression/Zlib.hpp"
#include "sculk/protocol/connection/coro/Scheduler.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include <PacketPriority.h>
#include <RakNetStatistics.h>
#include <chrono>
#include <limits>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace {

constexpr std::uint8_t  MINECRAFT_BATCH_PACKET_ID      = 0xFE;
constexpr std::uint64_t MANUAL_RECEIVE_PUMP_GENERATION = std::numeric_limits<std::uint64_t>::max();
constexpr std::size_t   MAX_BATCH_DECOMPRESSED_BYTES   = 8U * 1024U * 1024U;
constexpr std::size_t   MAX_BATCH_PACKET_BYTES         = 2U * 1024U * 1024U;
constexpr std::uint64_t MAX_INBOUND_QUEUED_BYTES       = 64ULL * 1024ULL * 1024ULL;
constexpr std::uint64_t MAX_OUTBOUND_QUEUED_BYTES      = 64ULL * 1024ULL * 1024ULL;

void subtractQueuedBytesSaturating(std::atomic_uint64_t& counter, std::uint64_t bytes) noexcept {
    auto current = counter.load(std::memory_order_relaxed);
    while (true) {
        const auto next = (bytes >= current) ? 0ULL : (current - bytes);
        if (counter.compare_exchange_weak(current, next, std::memory_order_acq_rel, std::memory_order_relaxed)) {
            return;
        }
    }
}

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

[[nodiscard]] bool isReceivePumpCancelled(const Session& session, std::uint64_t expectedGeneration) noexcept {
    if (expectedGeneration == MANUAL_RECEIVE_PUMP_GENERATION) {
        return false;
    }

    return session.receivePumpGeneration() != expectedGeneration;
}

[[nodiscard]] PacketBuffer finalizeBatchedPayload(
    PacketBuffer&&                                 packetsBuffer,
    const std::optional<Session::CompressionType>& compressionType,
    std::size_t                                    compressionThreshold
) {
    PacketBuffer finalBuffer{};
    BinaryStream compressedStream{finalBuffer};

    if (compressionType.has_value()) {
        auto headerType = Session::CompressionType::None;
        if (packetsBuffer.size() >= compressionThreshold) {
            headerType = *compressionType;
            switch (headerType) {
            case Session::CompressionType::Zlib: {
                packetsBuffer = compression::zlib::compress(packetsBuffer);
                break;
            }
            case Session::CompressionType::Snappy: {
                packetsBuffer = compression::snappy::compress(packetsBuffer);
                break;
            }
            default:
                break;
            }
        }

        compressedStream.writeEnum(headerType, &BinaryStream::writeSignedChar);
    }

    compressedStream.writeAndMoveBuffer(std::move(packetsBuffer));

    return finalBuffer;
}

class ReceiveNotificationAwaitable final {
public:
    ReceiveNotificationAwaitable(Session& session, std::uint64_t expectedGeneration) noexcept
    : mSession(session),
      mExpectedGeneration(expectedGeneration) {}

    [[nodiscard]] bool await_ready() const noexcept {
        return mSession.hasPendingInboundPackets() || isReceivePumpCancelled(mSession, mExpectedGeneration);
    }

    bool await_suspend(std::coroutine_handle<> handle) noexcept {
        mSession.registerReceiveWaiter(handle);
        if (mSession.hasPendingInboundPackets() || isReceivePumpCancelled(mSession, mExpectedGeneration)) {
            mSession.notifyOneReceiver();
        }
        return true;
    }

    void await_resume() const noexcept {}

private:
    Session&      mSession;
    std::uint64_t mExpectedGeneration;
};

} // namespace

Session::Session(RakNet::RakPeerInterface* peer, RakNet::AddressOrGUID remote, coro::Scheduler* scheduler) noexcept
: mPeer(peer),
  mRemote(remote),
  mScheduler(scheduler) {}

bool Session::sendPacket(const IPacket& packet) {
    std::vector<std::byte> buffer{};
    BinaryStream           stream{buffer};
    packet.writeWithHeader(stream);
    return sendPacketBuffer(std::move(buffer));
}

bool Session::sendPacketImmediately(const IPacket& packet) {
    std::vector<std::byte> buffer{};
    BinaryStream           stream{buffer};
    packet.writeWithHeader(stream);
    return sendPacketBufferImmediately(std::move(buffer));
}

bool Session::sendPacketBuffer(std::span<const std::byte> buffer) {
    if (!mConnected.load(std::memory_order_relaxed)) {
        return false;
    }

    const auto packetBytes = static_cast<std::uint64_t>(buffer.size());
    const auto previous    = mOutboundQueuedBytes.fetch_add(packetBytes, std::memory_order_acq_rel);
    if (previous + packetBytes > MAX_OUTBOUND_QUEUED_BYTES) {
        mOutboundQueuedBytes.fetch_sub(packetBytes, std::memory_order_relaxed);
        return false;
    }

    OutboundPacket packet{};
    packet.mPayload.assign(buffer.begin(), buffer.end());
    const bool enqueued = mOutboundPackets.enqueue(std::move(packet));
    if (!enqueued) {
        mOutboundQueuedBytes.fetch_sub(packetBytes, std::memory_order_relaxed);
    }
    return enqueued;
}

bool Session::sendPacketBuffer(std::vector<std::byte>&& buffer) {
    if (!mConnected.load(std::memory_order_relaxed)) {
        return false;
    }

    const auto packetBytes = static_cast<std::uint64_t>(buffer.size());
    const auto previous    = mOutboundQueuedBytes.fetch_add(packetBytes, std::memory_order_acq_rel);
    if (previous + packetBytes > MAX_OUTBOUND_QUEUED_BYTES) {
        mOutboundQueuedBytes.fetch_sub(packetBytes, std::memory_order_relaxed);
        return false;
    }

    OutboundPacket packet{};
    packet.mPayload     = std::move(buffer);
    const bool enqueued = mOutboundPackets.enqueue(std::move(packet));
    if (!enqueued) {
        mOutboundQueuedBytes.fetch_sub(packetBytes, std::memory_order_relaxed);
    }
    return enqueued;
}

std::uint32_t
Session::sendPacketBufferImmediately(std::span<const std::byte> buffer, std::uint32_t forceReceiptNumber) {
    if (!mConnected.load(std::memory_order_relaxed) || !mPeer || buffer.empty()) {
        return 0;
    }

    PacketBuffer packetsBuffer{};
    packetsBuffer.reserve(buffer.size() + 5);
    BinaryStream packetStream{packetsBuffer};
    packetStream.writeUnsignedVarInt(static_cast<std::uint32_t>(buffer.size()));
    packetStream.writeBytes(buffer.data(), buffer.size());

    auto batched = finalizeBatchedPayload(std::move(packetsBuffer), mCompressionType, mCompressionThreshold);

    if (isEncrypted()) {
        batched = mEncryption->encrypt(batched);
    }

    if (batched.empty()) {
        return 0;
    }

    PacketBuffer framed{};
    framed.reserve(batched.size() + 1);
    framed.push_back(static_cast<std::byte>(MINECRAFT_BATCH_PACKET_ID));
    framed.insert(framed.end(), batched.begin(), batched.end());
    return sendBatchedPacketBufferImmediately(framed, forceReceiptNumber);
}

coro::Task<Result<std::vector<std::byte>>> Session::receivePacketBufferAsync() {
    co_return co_await receivePacketBufferAsync(MANUAL_RECEIVE_PUMP_GENERATION);
}

coro::Task<Result<std::vector<std::byte>>> Session::receivePacketBufferAsync(std::uint64_t expectedGeneration) {
    std::vector<std::byte> output;
    for (;;) {
        if (isReceivePumpCancelled(*this, expectedGeneration)) {
            co_return error_utils::makeError("receive pump cancelled");
        }

        if (receivePacketBuffer(output)) {
            // Once dequeued, keep packet order stable by returning the packet directly.
            co_return std::move(output);
        }

        if (!isConnected()) {
            co_return error_utils::makeError("session disconnected");
        }

        co_await ReceiveNotificationAwaitable{*this, expectedGeneration};
    }
}

std::uint32_t Session::sendBatchedPacketBufferImmediately(
    std::span<const std::byte> buffer,
    std::uint32_t              forceReceiptNumber
) noexcept {
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

bool Session::receivePacketBuffer(std::vector<std::byte>& outBuffer) noexcept {
    std::vector<std::byte> packet;
    if (!mInboundPackets.try_dequeue(packet)) {
        return false;
    }

    subtractQueuedBytesSaturating(mInboundQueuedBytes, static_cast<std::uint64_t>(packet.size()));
    outBuffer = std::move(packet);
    return true;
}

bool Session::hasPendingInboundPackets() const noexcept { return mInboundPackets.size_approx() > 0; }

bool Session::hasPendingOutboundPackets() const noexcept { return mOutboundPackets.size_approx() > 0; }

bool Session::tryMarkOutboundDirty() noexcept {
    bool expected = false;
    return mOutboundDirty.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
}

void Session::clearOutboundDirty() noexcept { mOutboundDirty.store(false, std::memory_order_release); }

bool Session::isConnected() const noexcept { return mConnected.load(std::memory_order_relaxed); }

RakNet::RakNetGUID Session::getGuid() const noexcept { return mRemote.rakNetGuid; }

RakNet::SystemAddress Session::getSystemAddress() const noexcept { return mRemote.systemAddress; }

RakNet::AddressOrGUID Session::getEndpoint() const noexcept { return mRemote; }

#define SCULK_RAKNET_FLAG(x) static_cast<std::uint32_t>(RakNet::RNSPerSecondMetrics::x)

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

    status.mUserMessageBytesPushedPerSecond = stats->valueOverLastSecond[SCULK_RAKNET_FLAG(USER_MESSAGE_BYTES_PUSHED)];
    status.mUserMessageBytesSentPerSecond   = stats->valueOverLastSecond[SCULK_RAKNET_FLAG(USER_MESSAGE_BYTES_SENT)];
    status.mUserMessageBytesResentPerSecond = stats->valueOverLastSecond[SCULK_RAKNET_FLAG(USER_MESSAGE_BYTES_RESENT)];
    status.mUserMessageBytesReceivedPerSecond =
        stats->valueOverLastSecond[SCULK_RAKNET_FLAG(USER_MESSAGE_BYTES_RECEIVED_PROCESSED)];
    status.mActualBytesSentPerSecond     = stats->valueOverLastSecond[SCULK_RAKNET_FLAG(ACTUAL_BYTES_SENT)];
    status.mActualBytesReceivedPerSecond = stats->valueOverLastSecond[SCULK_RAKNET_FLAG(ACTUAL_BYTES_RECEIVED)];

    status.mUserMessageBytesSentTotal = stats->runningTotal[SCULK_RAKNET_FLAG(USER_MESSAGE_BYTES_SENT)];
    status.mUserMessageBytesReceivedTotal =
        stats->runningTotal[SCULK_RAKNET_FLAG(USER_MESSAGE_BYTES_RECEIVED_PROCESSED)];
    status.mActualBytesSentTotal     = stats->runningTotal[SCULK_RAKNET_FLAG(ACTUAL_BYTES_SENT)];
    status.mActualBytesReceivedTotal = stats->runningTotal[SCULK_RAKNET_FLAG(ACTUAL_BYTES_RECEIVED)];

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

    std::vector<std::byte> inboundPacket;
    std::uint64_t          drainedInboundBytes = 0;
    while (mInboundPackets.try_dequeue(inboundPacket)) {
        drainedInboundBytes += static_cast<std::uint64_t>(inboundPacket.size());
        inboundPacket.clear();
    }
    subtractQueuedBytesSaturating(mInboundQueuedBytes, drainedInboundBytes);

    OutboundPacket outboundPacket;
    std::uint64_t  drainedOutboundBytes = 0;
    while (mOutboundPackets.try_dequeue(outboundPacket)) {
        drainedOutboundBytes += static_cast<std::uint64_t>(outboundPacket.mPayload.size());
        outboundPacket.mPayload.clear();
    }
    subtractQueuedBytesSaturating(mOutboundQueuedBytes, drainedOutboundBytes);

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

std::uint64_t Session::receivePumpGeneration() const noexcept {
    return mReceivePumpGeneration.load(std::memory_order_acquire);
}

void Session::cancelReceiveWaiters() noexcept {
    mReceivePumpGeneration.fetch_add(1, std::memory_order_acq_rel);

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

    const auto packetBytes = static_cast<std::uint64_t>(buffer.size());
    const auto previous    = mInboundQueuedBytes.fetch_add(packetBytes, std::memory_order_acq_rel);
    if (previous + packetBytes > MAX_INBOUND_QUEUED_BYTES) {
        mInboundQueuedBytes.fetch_sub(packetBytes, std::memory_order_relaxed);
        return false;
    }

    const bool ok = mInboundPackets.enqueue(std::move(buffer));
    if (!ok) {
        mInboundQueuedBytes.fetch_sub(packetBytes, std::memory_order_relaxed);
    }
    if (ok) {
        notifyOneReceiver();
    }
    return ok;
}

bool Session::tryDequeueOutboundPacket(OutboundPacket& outPacket) noexcept {
    if (!mOutboundPackets.try_dequeue(outPacket)) {
        return false;
    }

    subtractQueuedBytesSaturating(mOutboundQueuedBytes, static_cast<std::uint64_t>(outPacket.mPayload.size()));
    return true;
}

std::size_t Session::tryDequeueAllOutboundPackets(OutboundBatch& outPackets) noexcept {
    outPackets.clear();

    OutboundPacket packet;
    std::uint64_t  drainedBytes = 0;
    while (mOutboundPackets.try_dequeue(packet)) {
        drainedBytes += static_cast<std::uint64_t>(packet.mPayload.size());
        outPackets.push_back(std::move(packet));
    }

    subtractQueuedBytesSaturating(mOutboundQueuedBytes, drainedBytes);

    return outPackets.size();
}

PacketBuffer Session::serializeBatchedPackets(const PacketBufferBatch& packets) {
    PacketBuffer packetsBuffer{};
    BinaryStream packetStream{packetsBuffer};
    for (const auto& packet : packets) {
        packetStream.writeUnsignedVarInt(static_cast<std::uint32_t>(packet.size()));
        packetStream.writeBytes(packet.data(), packet.size());
    }

    auto finalBuffer = finalizeBatchedPayload(std::move(packetsBuffer), mCompressionType, mCompressionThreshold);

    if (isEncrypted()) {
        finalBuffer = mEncryption->encrypt(finalBuffer);
    }

    return finalBuffer;
}

Result<PacketBufferBatch> Session::deserializeBatchPackets(std::span<const std::byte> batchedBuffer) {

    if (isEncrypted()) {
        auto decrypted = mEncryption->decrypt(batchedBuffer);

        batchedBuffer = decrypted;
    }

    ReadOnlyBinaryStream compressedStream{batchedBuffer};
    PacketBuffer         decompressedBuffer{};

    if (isCompressed()) {
        CompressionType type{};
        if (!compressedStream.readEnum(type, &ReadOnlyBinaryStream::readSignedChar)) {
            return error_utils::makeError("failed to read compression type from batch packet");
        }

        const auto   compressedPayload = compressedStream.getLeftBufferView();
        PacketBuffer compressedBuffer(compressedPayload.begin(), compressedPayload.end());
        switch (type) {
        case CompressionType::Zlib: {
            auto res = compression::zlib::decompress(compressedBuffer);
            if (!res) {
                return error_utils::makeError("zlib decompression failed");
            }
            decompressedBuffer = std::move(*res);
            break;
        }
        case CompressionType::Snappy: {
            auto res = compression::snappy::decompress(compressedBuffer);
            if (!res) {
                return error_utils::makeError("snappy decompression failed");
            }
            decompressedBuffer = std::move(*res);
            break;
        }
        default:
            decompressedBuffer = std::move(compressedBuffer);
            break;
        }
    } else {
        decompressedBuffer.assign(batchedBuffer.begin(), batchedBuffer.end());
    }

    if (decompressedBuffer.size() > MAX_BATCH_DECOMPRESSED_BYTES) {
        return error_utils::makeError("batched payload exceeds configured size limit");
    }

    ReadOnlyBinaryStream stream{decompressedBuffer};
    PacketBufferBatch    packets{};

    while (stream.hasDataLeft()) {
        std::uint32_t packetSize{};
        if (!stream.readUnsignedVarInt(packetSize)) {
            return error_utils::makeError("failed to read batched packet size");
        }
        if (packetSize > MAX_BATCH_PACKET_BYTES) {
            return error_utils::makeError("batched packet exceeds configured size limit");
        }
        const auto remaining = stream.size() - stream.getPosition();
        if (static_cast<std::size_t>(packetSize) > remaining) {
            return error_utils::makeError("batched packet size exceeds remaining payload");
        }
        std::vector<std::byte> packetData(packetSize, std::byte{});
        if (!stream.readBytes(packetData.data(), packetSize)) {
            return error_utils::makeError("failed to read batched packet payload");
        }
        packets.push_back(std::move(packetData));
    }

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

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
