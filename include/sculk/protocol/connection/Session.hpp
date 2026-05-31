// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/connection/NetworkStatus.hpp"
#include "sculk/protocol/connection/coro/Task.hpp"
#include "sculk/protocol/connection/encryption/CryptoManager.hpp"
#include "sculk/protocol/utility/Result.hpp"
#include <RakPeerInterface.h>
#include <atomic>
#include <concurrentqueue.h>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>

namespace sculk::protocol::inline abi_v975 {

namespace coro {
class Scheduler;
}

struct OutboundPacket {
    std::vector<std::byte> mPayload{};
};

using PacketBuffer      = std::vector<std::byte>;
using PacketBufferBatch = std::vector<PacketBuffer>;
using OutboundBatch     = std::vector<OutboundPacket>;

class Session {
public:
    enum class CompressionType : std::int8_t {
        None   = -1,
        Zlib   = 0,
        Snappy = 1,
    };

protected:
    RakNet::RakPeerInterface*                            mPeer{};
    RakNet::AddressOrGUID                                mRemote{};
    coro::Scheduler*                                     mScheduler{};
    moodycamel::ConcurrentQueue<std::vector<std::byte>>  mInboundPackets{};
    moodycamel::ConcurrentQueue<OutboundPacket>          mOutboundPackets{};
    moodycamel::ConcurrentQueue<std::coroutine_handle<>> mReceiveWaiters{};
    std::atomic_bool                                     mConnected{true};
    std::atomic_bool                                     mOutboundDirty{false};
    std::atomic_uint64_t                                 mInboundQueuedBytes{0};
    std::atomic_uint64_t                                 mOutboundQueuedBytes{0};
    std::optional<CompressionType>                       mCompressionType{};
    std::size_t                                          mCompressionThreshold{};
    std::optional<CryptoManager>                         mEncryption{};

public:
    explicit Session(RakNet::RakPeerInterface* peer, RakNet::AddressOrGUID remote, coro::Scheduler* scheduler) noexcept;

    Session(const Session&)            = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&)                 = delete;
    Session& operator=(Session&&)      = delete;

public:
    virtual ~Session() = default;

public:
    [[nodiscard]] bool isCompressed() const noexcept { return mCompressionType.has_value(); }

    void setCompressionType(CompressionType type) noexcept { mCompressionType = type; }

    [[nodiscard]] std::size_t getCompressionThreshold() const noexcept { return mCompressionThreshold; }

    void setCompressionThreshold(std::size_t threshold) noexcept { mCompressionThreshold = threshold; }

    [[nodiscard]] bool isEncrypted() const noexcept { return mEncryption.has_value(); }

    void setEncrypted(std::vector<std::byte> key) noexcept { mEncryption.emplace(std::move(key)); }

    // Queue-based send. Network I/O thread flushes the queue later.
    [[nodiscard]] bool sendPacket(std::span<const std::byte> buffer);

    // Move-friendly queue-based send.
    [[nodiscard]] bool sendPacket(std::vector<std::byte>&& buffer);

    // Immediate send path for one logical packet. The packet is still wrapped into a batch before transport send.
    [[nodiscard]] std::uint32_t
    sendPacketImmediately(std::span<const std::byte> buffer, std::uint32_t forceReceiptNumber = 0);

    // Low-level immediate send for payloads that are already fully serialized for transport.
    [[nodiscard]] std::uint32_t
    sendRawPacketImmediately(std::span<const std::byte> buffer, std::uint32_t forceReceiptNumber = 0) noexcept;

    // Non-blocking receive. Returns false when queue is empty.
    [[nodiscard]] bool receivePacket(std::vector<std::byte>& outBuffer) noexcept;

    // Coroutine receive. Suspends until one packet is available.
    // Returns error when session is disconnected before new packet arrives.
    [[nodiscard]] coro::Task<Result<std::vector<std::byte>>> receivePacketAsync();

    [[nodiscard]] coro::Task<Result<std::vector<std::byte>>> receivePacketAsync(std::uint64_t expectedGeneration);

    [[nodiscard]] bool hasPendingInboundPackets() const noexcept;

    [[nodiscard]] bool hasPendingOutboundPackets() const noexcept;

    [[nodiscard]] bool tryMarkOutboundDirty() noexcept;

    void clearOutboundDirty() noexcept;

    [[nodiscard]] bool isConnected() const noexcept;

    [[nodiscard]] RakNet::RakNetGUID guid() const noexcept;

    [[nodiscard]] RakNet::AddressOrGUID remoteEndpoint() const noexcept;

    [[nodiscard]] NetworkStatus getNetworkStatus() const noexcept;

    void markDisconnected() noexcept;

    bool enqueueInboundPacket(std::vector<std::byte>&& buffer) noexcept;

    bool tryDequeueOutboundPacket(OutboundPacket& outPacket) noexcept;

    std::size_t tryDequeueAllOutboundPackets(OutboundBatch& outPackets) noexcept;

    [[nodiscard]] PacketBuffer serializeBatchedPackets(const PacketBufferBatch& packets);

    [[nodiscard]] Result<PacketBufferBatch> deserializeBatchPackets(std::span<const std::byte> batchedBuffer);

    void notifyOneReceiver() noexcept;

    void registerReceiveWaiter(std::coroutine_handle<> handle) noexcept;

    [[nodiscard]] std::uint64_t receivePumpGeneration() const noexcept;

    void cancelReceiveWaiters() noexcept;

private:
    std::atomic_uint64_t mReceivePumpGeneration{0};
};

} // namespace sculk::protocol::inline abi_v975
