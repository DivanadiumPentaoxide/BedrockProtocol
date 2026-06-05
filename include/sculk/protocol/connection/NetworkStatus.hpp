// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <RakNetTypes.h>
#include <cstddef>
#include <cstdint>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class Session;

class NetworkStatus final {
public:
    enum class ConnectionState : std::uint8_t {
        Disconnected,
        Connecting,
        Connected,
        Disconnecting,
        Unknown,
    };

    [[nodiscard]] std::uint64_t getSampledAtMs() const noexcept { return mSampledAtMs; }

    [[nodiscard]] bool getConnected() const noexcept { return mConnected; }

    [[nodiscard]] ConnectionState getConnectionState() const noexcept { return mConnectionState; }

    [[nodiscard]] int getAveragePingMs() const noexcept { return mAveragePingMs; }

    [[nodiscard]] int getLastPingMs() const noexcept { return mLastPingMs; }

    [[nodiscard]] int getLowestPingMs() const noexcept { return mLowestPingMs; }

    [[nodiscard]] float getPacketLossLastSecond() const noexcept { return mPacketLossLastSecond; }

    [[nodiscard]] float getPacketLossTotal() const noexcept { return mPacketLossTotal; }

    [[nodiscard]] std::uint64_t getUserMessageBytesPushedPerSecond() const noexcept {
        return mUserMessageBytesPushedPerSecond;
    }

    [[nodiscard]] std::uint64_t getUserMessageBytesSentPerSecond() const noexcept {
        return mUserMessageBytesSentPerSecond;
    }

    [[nodiscard]] std::uint64_t getUserMessageBytesResentPerSecond() const noexcept {
        return mUserMessageBytesResentPerSecond;
    }

    [[nodiscard]] std::uint64_t getUserMessageBytesReceivedPerSecond() const noexcept {
        return mUserMessageBytesReceivedPerSecond;
    }

    [[nodiscard]] std::uint64_t getActualBytesSentPerSecond() const noexcept { return mActualBytesSentPerSecond; }

    [[nodiscard]] std::uint64_t getActualBytesReceivedPerSecond() const noexcept {
        return mActualBytesReceivedPerSecond;
    }

    [[nodiscard]] std::uint64_t getUserMessageBytesSentTotal() const noexcept { return mUserMessageBytesSentTotal; }

    [[nodiscard]] std::uint64_t getUserMessageBytesReceivedTotal() const noexcept {
        return mUserMessageBytesReceivedTotal;
    }

    [[nodiscard]] std::uint64_t getActualBytesSentTotal() const noexcept { return mActualBytesSentTotal; }

    [[nodiscard]] std::uint64_t getActualBytesReceivedTotal() const noexcept { return mActualBytesReceivedTotal; }

    [[nodiscard]] std::uint64_t getBytesQueuedForSend() const noexcept { return mBytesQueuedForSend; }

    [[nodiscard]] std::uint64_t getBytesInResendBuffer() const noexcept { return mBytesInResendBuffer; }

    [[nodiscard]] std::uint32_t getMessagesInResendBuffer() const noexcept { return mMessagesInResendBuffer; }

    [[nodiscard]] std::size_t getInboundQueueApprox() const noexcept { return mInboundQueueApprox; }

    [[nodiscard]] std::size_t getOutboundQueueApprox() const noexcept { return mOutboundQueueApprox; }

    [[nodiscard]] bool getHasTransportStatistics() const noexcept { return mHasTransportStatistics; }

    [[nodiscard]] bool getLimitedByCongestionControl() const noexcept { return mLimitedByCongestionControl; }

    [[nodiscard]] bool getLimitedByOutgoingBandwidthLimit() const noexcept { return mLimitedByOutgoingBandwidthLimit; }

    [[nodiscard]] std::uint64_t getCongestionControlBpsLimit() const noexcept { return mCongestionControlBpsLimit; }

    [[nodiscard]] std::uint64_t getOutgoingBandwidthBpsLimit() const noexcept { return mOutgoingBandwidthBpsLimit; }

    [[nodiscard]] RakNet::RakNetGUID getGuid() const noexcept { return mGuid; }

private:
    friend class Session;

    // Snapshot timestamp (milliseconds since Unix epoch).
    std::uint64_t   mSampledAtMs{};
    bool            mConnected{};
    ConnectionState mConnectionState{ConnectionState::Unknown};
    // Ping metrics (milliseconds). -1 means unavailable.
    int mAveragePingMs{-1};
    int mLastPingMs{-1};
    int mLowestPingMs{-1};
    // Packet loss ratio in [0, 1].
    float mPacketLossLastSecond{};
    float mPacketLossTotal{};
    // Throughput and traffic counters (bytes).
    std::uint64_t mUserMessageBytesPushedPerSecond{};
    std::uint64_t mUserMessageBytesSentPerSecond{};
    std::uint64_t mUserMessageBytesResentPerSecond{};
    std::uint64_t mUserMessageBytesReceivedPerSecond{};
    std::uint64_t mActualBytesSentPerSecond{};
    std::uint64_t mActualBytesReceivedPerSecond{};
    std::uint64_t mUserMessageBytesSentTotal{};
    std::uint64_t mUserMessageBytesReceivedTotal{};
    std::uint64_t mActualBytesSentTotal{};
    std::uint64_t mActualBytesReceivedTotal{};
    // Internal send buffer pressure.
    std::uint64_t mBytesQueuedForSend{};
    std::uint64_t mBytesInResendBuffer{};
    std::uint32_t mMessagesInResendBuffer{};
    // Local application queue pressure.
    std::size_t   mInboundQueueApprox{};
    std::size_t   mOutboundQueueApprox{};
    bool          mHasTransportStatistics{};
    bool          mLimitedByCongestionControl{};
    bool          mLimitedByOutgoingBandwidthLimit{};
    std::uint64_t mCongestionControlBpsLimit{};
    std::uint64_t mOutgoingBandwidthBpsLimit{};
    // Peer GUID.
    RakNet::RakNetGUID mGuid{};
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
