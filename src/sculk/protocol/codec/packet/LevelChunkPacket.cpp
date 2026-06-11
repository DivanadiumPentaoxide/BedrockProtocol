// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/LevelChunkPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds LevelChunkPacket::getId() const noexcept { return MinecraftPacketIds::LevelChunk; }

std::string_view LevelChunkPacket::getName() const noexcept { return "LevelChunkPacket"; }

void LevelChunkPacket::write(BinaryStream& stream) const {
    mPosition.write(stream);
    stream.writeVarInt(mDimensionId);

    if (mClientNeedsToRequestSubchunks) {
        if (mClientRequestSubChunkLimit < 0) {
            stream.writeUnsignedVarInt(0xFFFFFFFFu);
        } else {
            stream.writeUnsignedVarInt(0xFFFFFFFEu);
            stream.writeSignedShort(mClientRequestSubChunkLimit);
        }
    } else {
        stream.writeUnsignedVarInt(mSubChunksCount);
    }

    stream.writeOptional(mCacheBlobs, [&](BinaryStream& stream, const std::vector<std::uint64_t>& blobs) {
        stream.writeArray(blobs, &BinaryStream::writeUnsignedInt64);
    });

    stream.writeString(mSerializedChunk);
}

Result<> LevelChunkPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readVarInt(mDimensionId));

    std::uint32_t subChunkCount{};
    _SCULK_READ(stream.readUnsignedVarInt(subChunkCount));

    if (subChunkCount == 0xFFFFFFFEu) {
        mClientNeedsToRequestSubchunks = true;
        _SCULK_READ(stream.readSignedShort(mClientRequestSubChunkLimit));
    } else {
        if (subChunkCount == 0xFFFFFFFFu) {
            mClientNeedsToRequestSubchunks = true;
            mSubChunksCount                = 0;
            mClientRequestSubChunkLimit    = -1;
        } else {
            mClientNeedsToRequestSubchunks = false;
            mSubChunksCount                = subChunkCount;
            mClientRequestSubChunkLimit    = 0;
        }
    }

    _SCULK_READ(stream.readOptional(mCacheBlobs, [&](ReadOnlyBinaryStream& stream, std::vector<std::uint64_t>& blobs) {
        return stream.readArray(blobs, &ReadOnlyBinaryStream::readUnsignedInt64);
    }));

    return stream.readString(mSerializedChunk);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LevelChunkPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mDimensionId),
        SCULK_FORMAT_FIELD(mIsChunkInTickRange),
        SCULK_FORMAT_FIELD(mSerializedChunk),
        SCULK_FORMAT_FIELD(mSubChunksCount),
        SCULK_FORMAT_FIELD(mClientNeedsToRequestSubchunks),
        SCULK_FORMAT_FIELD(mClientRequestSubChunkLimit),
        SCULK_FORMAT_FIELD(mCacheBlobs)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
