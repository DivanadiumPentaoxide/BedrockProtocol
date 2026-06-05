// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/Session.hpp"
#include "sculk/protocol/connection/compression/Snappy.hpp"
#include "sculk/protocol/connection/compression/Zlib.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include <PacketPriority.h>
#include <RakNetStatistics.h>
#include <chrono>
#include <thread>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace {

constexpr std::uint8_t MINECRAFT_BATCH_PACKET_ID = 0xFE;

constexpr auto FLUSH_INTERVAL = std::chrono::milliseconds(20);

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

} // namespace

Session::Session(RakNet::RakPeerInterface* peer, const RakNet::AddressOrGUID& remote) noexcept
: mPeer(peer),
  mRemote(remote),
  mConnected(true),
  mNextFlushAt(std::chrono::steady_clock::now() + FLUSH_INTERVAL) {}

Session::~Session() { disconnect(); }

bool Session::isCompressed() const noexcept { return mCompressionType.has_value(); }

void Session::setCompressionType(CompressionType type) noexcept {
    std::scoped_lock lock{mMutex};
    mCompressionType = type;
}

void Session::setCompressionThreshold(std::size_t threshold) noexcept {
    std::scoped_lock lock{mMutex};
    mCompressionThreshold = threshold;
}

bool Session::isEncrypted() const noexcept { return mEncryption.has_value(); }

void Session::setEncrypted(std::vector<std::byte> key) noexcept {
    std::scoped_lock lock{mMutex};
    mEncryption.emplace(std::move(key));
}

bool Session::sendPacket(BufferView buffer) { return mOutboundPackets.enqueue(Buffer{buffer.begin(), buffer.end()}); }

bool Session::sendPacket(Buffer&& buffer) { return mOutboundPackets.enqueue(std::move(buffer)); }

bool Session::sendPacketImmediately(Buffer&& buffer) {
    Buffer packetsBuffer{};
    packetsBuffer.reserve(buffer.size() + 5);
    BinaryStream packetStream{packetsBuffer};
    packetStream.writeUnsignedVarInt(static_cast<std::uint32_t>(buffer.size()));
    packetStream.writeAndMoveBuffer(std::move(buffer));

    return sendBatchedBufferImmediately(std::move(packetsBuffer));
}

bool Session::sendPacketImmediately(BufferView buffer) {
    Buffer packetsBuffer{};
    packetsBuffer.reserve(buffer.size() + 5);
    BinaryStream packetStream{packetsBuffer};
    packetStream.writeUnsignedVarInt(static_cast<std::uint32_t>(buffer.size()));
    packetStream.writeBytes(buffer.data(), buffer.size());

    return sendBatchedBufferImmediately(std::move(packetsBuffer));
}

bool Session::flush() {
    std::scoped_lock lock{mMutex};
    mNextFlushAt = std::chrono::steady_clock::now() + FLUSH_INTERVAL;
    return flushUnlocked();
}

bool Session::flushIfDue(std::chrono::steady_clock::time_point now) noexcept {
    if (!mConnected.load(std::memory_order_relaxed) || !mPeer) {
        return false;
    }

    if (now < mNextFlushAt) {
        return false;
    }

    std::scoped_lock lock{mMutex};
    if (now < mNextFlushAt) {
        return false;
    }

    mNextFlushAt = now + FLUSH_INTERVAL;
    return flushUnlocked();
}

bool Session::flushUnlocked() {
    if (!mConnected.load(std::memory_order_relaxed) || !hasPendingOutboundPackets() || !mPeer) {
        return false;
    }

    std::vector<Buffer> outPackets{};
    outPackets.reserve(mOutboundPackets.size_approx());

    Buffer packet{};
    while (mOutboundPackets.try_dequeue(packet)) {
        outPackets.push_back(std::move(packet));
    }

    if (outPackets.empty()) {
        return false;
    }

    Buffer       packetsBuffer{};
    BinaryStream packetStream{packetsBuffer};

    for (const auto& buf : outPackets) {
        packetStream.writeUnsignedVarInt(static_cast<std::uint32_t>(buf.size()));
        packetStream.writeBytes(buf.data(), buf.size());
    }

    return sendBatchedBufferImmediately(std::move(packetsBuffer));
}

bool Session::receivePacket(Buffer& outBuffer) noexcept {
    Buffer packet{};
    if (!mInboundPackets.try_dequeue(packet)) {
        return false;
    }

    outBuffer = std::move(packet);
    return true;
}


coro::Task<Result<Session::Buffer>> Session::receivePacketAsync() {
    Buffer output{};
    while (true) {
        if (!isConnected()) {
            co_return error_utils::makeError("session disconnected");
        }

        if (receivePacket(output)) {
            co_return std::move(output);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool Session::sendBatchedBufferImmediately(Buffer&& packetsBuffer) noexcept {
    Buffer       finalBuffer{};
    BinaryStream compressedStream{finalBuffer};

    std::optional<CompressionType> compressionType{};
    std::size_t                    compressionThreshold{};
    {
        std::scoped_lock lock{mMutex};
        compressionType      = mCompressionType;
        compressionThreshold = mCompressionThreshold;
    }

    if (compressionType.has_value()) {
        auto headerType = CompressionType::None;
        if (packetsBuffer.size() >= compressionThreshold) {
            headerType = *compressionType;
            switch (headerType) {
            case CompressionType::Zlib: {
                packetsBuffer = compression::zlib::compress(packetsBuffer);
                break;
            }
            case CompressionType::Snappy: {
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

    {
        std::scoped_lock lock{mMutex};
        if (mEncryption.has_value()) {
            finalBuffer = mEncryption->encrypt(finalBuffer);
        }
    }

    if (finalBuffer.empty()) {
        return false;
    }

    Buffer framed{};
    framed.reserve(finalBuffer.size() + 1);
    framed.push_back(static_cast<std::byte>(MINECRAFT_BATCH_PACKET_ID));
    framed.insert(framed.end(), finalBuffer.begin(), finalBuffer.end());

    return mPeer->Send(
        reinterpret_cast<const char*>(framed.data()),
        static_cast<int>(framed.size()),
        HIGH_PRIORITY,
        RELIABLE_ORDERED,
        0,
        mRemote,
        false,
        0
    );
}

bool Session::hasPendingInboundPackets() const noexcept { return mInboundPackets.size_approx() > 0; }

bool Session::hasPendingOutboundPackets() const noexcept { return mOutboundPackets.size_approx() > 0; }

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

void Session::disconnect() noexcept {
    const bool wasConnected = mConnected.exchange(false, std::memory_order_relaxed);

    {
        std::scoped_lock lock{mMutex};
        mCompressionThreshold = 0;
        mCompressionType.reset();
        mEncryption.reset();
    }

    if (!wasConnected) {
        return;
    }

    if (mPeer) {
        mPeer->CloseConnection(mRemote, true, 0, LOW_PRIORITY);
    }
}

bool Session::enqueueInboundPacket(Buffer&& buffer) noexcept {
    if (!mConnected.load(std::memory_order_relaxed)) {
        return false;
    }

    return mInboundPackets.enqueue(std::move(buffer));
}

Session::Buffer Session::serializeBatchedPackets(const BatchedBuffer& packets) {
    Buffer       packetsBuffer{};
    BinaryStream packetStream{packetsBuffer};
    for (const auto& packet : packets) {
        packetStream.writeUnsignedVarInt(static_cast<std::uint32_t>(packet.size()));
        packetStream.writeBytes(packet.data(), packet.size());
    }

    Session::Buffer finalBuffer{};
    BinaryStream    compressedStream{finalBuffer};

    if (mCompressionType.has_value()) {
        auto headerType = Session::CompressionType::None;
        if (packetsBuffer.size() >= mCompressionThreshold) {
            headerType = *mCompressionType;
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

    if (mEncryption.has_value()) {
        finalBuffer = mEncryption->encrypt(finalBuffer);
    }

    return finalBuffer;
}

Result<Session::BatchedBuffer> Session::deserializeBatchPackets(std::span<const std::byte> batchedBuffer) {
    if (mEncryption.has_value()) {
        auto decrypted = mEncryption->decrypt(batchedBuffer);
        batchedBuffer  = decrypted;
    }

    ReadOnlyBinaryStream compressedStream{batchedBuffer};
    Buffer               decompressedBuffer{};

    if (mCompressionType.has_value()) {
        CompressionType type{};
        if (!compressedStream.readEnum(type, &ReadOnlyBinaryStream::readSignedChar)) {
            return error_utils::makeError("failed to read compression type from batch packet");
        }

        const auto compressedPayload = compressedStream.getLeftBufferView();
        Buffer     compressedBuffer(compressedPayload.begin(), compressedPayload.end());
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

    ReadOnlyBinaryStream stream{decompressedBuffer};
    BatchedBuffer        packets{};

    while (stream.hasDataLeft()) {
        std::uint32_t packetSize{};
        if (!stream.readUnsignedVarInt(packetSize)) {
            return error_utils::makeError("failed to read batched packet size");
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

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
