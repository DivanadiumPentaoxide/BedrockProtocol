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
#include <chrono>
#include <concurrentqueue.h>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <span>
#include <utility>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class Session {
public:
    using Buffer          = std::vector<std::byte>;
    using BufferView      = std::span<const std::byte>;
    using BatchedBuffer   = std::vector<Buffer>;
    using OutboundBuffers = std::vector<Buffer>;

public:
    enum class CompressionType : std::int8_t {
        None   = -1,
        Zlib   = 0,
        Snappy = 1,
    };

protected:
    RakNet::RakPeerInterface*             mPeer{};
    RakNet::AddressOrGUID                 mRemote{};
    moodycamel::ConcurrentQueue<Buffer>   mInboundPackets{};
    moodycamel::ConcurrentQueue<Buffer>   mOutboundPackets{};
    std::atomic_bool                      mConnected{};
    std::optional<CompressionType>        mCompressionType{};
    std::size_t                           mCompressionThreshold{};
    std::optional<CryptoManager>          mEncryption{};
    std::chrono::steady_clock::time_point mNextFlushAt{};
    mutable std::mutex                    mMutex{};

public:
    explicit Session(RakNet::RakPeerInterface* peer, const RakNet::AddressOrGUID& remote) noexcept;

    Session(const Session&)            = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&)                 = delete;
    Session& operator=(Session&&)      = delete;

    ~Session();

public:
    [[nodiscard]] bool isCompressed() const noexcept;

    void setCompressionType(CompressionType type) noexcept;

    void setCompressionThreshold(std::size_t threshold) noexcept;

    [[nodiscard]] bool isEncrypted() const noexcept;

    void setEncrypted(std::vector<std::byte> key) noexcept;

    bool sendPacket(Buffer&& Buffer);

    bool sendPacket(BufferView Buffer);

    bool sendPacketImmediately(Buffer&& Buffer);

    bool sendPacketImmediately(BufferView Buffer);

    [[nodiscard]] bool flush();

    [[nodiscard]] bool flushIfDue(std::chrono::steady_clock::time_point now) noexcept;

    [[nodiscard]] bool receivePacket(Buffer& outBuffer) noexcept;

    [[nodiscard]] coro::Task<Result<Buffer>> receivePacketAsync();

    void disconnect() noexcept;

    [[nodiscard]] bool isConnected() const noexcept;

    [[nodiscard]] RakNet::RakNetGUID getGuid() const noexcept;

    [[nodiscard]] RakNet::SystemAddress getSystemAddress() const noexcept;

    [[nodiscard]] RakNet::AddressOrGUID getEndpoint() const noexcept;

    [[nodiscard]] NetworkStatus getNetworkStatus() const noexcept;

public:
    [[nodiscard]] bool sendBatchedBufferImmediately(Buffer&& packetsBuffer) noexcept;

    [[nodiscard]] bool hasPendingInboundPackets() const noexcept;

    [[nodiscard]] bool hasPendingOutboundPackets() const noexcept;

    [[nodiscard]] bool enqueueInboundPacket(Buffer&& buffer) noexcept;

    [[nodiscard]] Buffer serializeBatchedPackets(const BatchedBuffer& packets);

    [[nodiscard]] Result<BatchedBuffer> deserializeBatchPackets(std::span<const std::byte> batchedBuffer);

private:
    [[nodiscard]] bool flushUnlocked();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
