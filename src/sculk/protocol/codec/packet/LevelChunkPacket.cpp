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
    if (!mClientNeedsToRequestSubchunks) {
        stream.writeUnsignedVarInt(mSubChunksCount);
    } else if (mClientRequestSubChunkLimit < 0) {
        stream.writeUnsignedVarInt(0xFFFFFFFFu);
    } else {
        stream.writeUnsignedVarInt(0xFFFFFFFEu);
        stream.writeUnsignedShort(static_cast<std::uint16_t>(mClientRequestSubChunkLimit));
    }
    stream.writeBool(mCacheEnabled);
    if (mCacheEnabled) {
        stream.writeArray(mCacheBlobs, &BinaryStream::writeUnsignedInt64);
    }
    stream.writeString(mSerializedChunk);
}

Result<> LevelChunkPacket::read(ReadOnlyBinaryStream& stream) {
    std::uint32_t subChunkHeader{};
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readVarInt(mDimensionId));
    _SCULK_READ(stream.readUnsignedVarInt(subChunkHeader));

    if (subChunkHeader == 0xFFFFFFFFu) {
        mClientNeedsToRequestSubchunks = true;
        mClientRequestSubChunkLimit    = -1;
        mSubChunksCount                = 0;
    } else if (subChunkHeader == 0xFFFFFFFEu) {
        std::uint16_t limit{};
        _SCULK_READ(stream.readUnsignedShort(limit));
        mClientNeedsToRequestSubchunks = true;
        mClientRequestSubChunkLimit    = static_cast<std::int16_t>(limit);
        mSubChunksCount                = 0;
    } else {
        mClientNeedsToRequestSubchunks = false;
        mSubChunksCount                = subChunkHeader;
        mClientRequestSubChunkLimit    = 0;
    }

    _SCULK_READ(stream.readBool(mCacheEnabled));
    if (mCacheEnabled) {
        _SCULK_READ(stream.readArray(mCacheBlobs, &ReadOnlyBinaryStream::readUnsignedInt64));
    } else {
        mCacheBlobs.clear();
    }

    return stream.readString(mSerializedChunk);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LevelChunkPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mDimensionId),
        SCULK_FORMAT_FIELD(mSerializedChunk),
        SCULK_FORMAT_FIELD(mSubChunksCount),
        SCULK_FORMAT_FIELD(mClientRequestSubChunkLimit),
        SCULK_FORMAT_FIELD(mCacheBlobs)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
