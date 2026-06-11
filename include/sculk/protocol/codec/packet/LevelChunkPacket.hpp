// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/level/chunk/ChunkPos.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class LevelChunkPacket : public IPacket {
public:
    ChunkPos                                  mPosition{};
    std::int32_t                              mDimensionId{};
    bool                                      mIsChunkInTickRange{};
    std::string                               mSerializedChunk{};
    std::uint32_t                             mSubChunksCount{};
    bool                                      mClientNeedsToRequestSubchunks{};
    std::int16_t                              mClientRequestSubChunkLimit{};
    std::optional<std::vector<std::uint64_t>> mCacheBlobs{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(LevelChunkPacket)