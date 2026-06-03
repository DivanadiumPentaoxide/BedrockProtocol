// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SubChunkPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void SubChunkPacket::SubChunkPosOffset::write(BinaryStream& stream) const {
    stream.writeSignedChar(mX);
    stream.writeSignedChar(mY);
    stream.writeSignedChar(mZ);
}

Result<> SubChunkPacket::SubChunkPosOffset::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedChar(mX));
    _SCULK_READ(stream.readSignedChar(mY));
    return stream.readSignedChar(mZ);
}

void SubChunkPacket::HeightmapData::write(BinaryStream& stream) const {
    stream.writeEnum(mHeightMapType, &BinaryStream::writeByte);
    if (mHeightMapType == HeightMapDataType::HasData) {
        stream.writeBytes(&mSubchunkHeightMap, sizeof(mSubchunkHeightMap));
    }
    stream.writeEnum(mRenderHeightMapType, &BinaryStream::writeByte);
    if (mRenderHeightMapType == HeightMapDataType::HasData) {
        stream.writeBytes(&mRenderHeightMap, sizeof(mRenderHeightMap));
    }
}

Result<> SubChunkPacket::HeightmapData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mHeightMapType, &ReadOnlyBinaryStream::readByte));
    if (mHeightMapType == HeightMapDataType::HasData) {
        _SCULK_READ(stream.readBytes(&mSubchunkHeightMap, sizeof(mSubchunkHeightMap)));
    }
    _SCULK_READ(stream.readEnum(mRenderHeightMapType, &ReadOnlyBinaryStream::readByte));
    if (mRenderHeightMapType == HeightMapDataType::HasData) {
        _SCULK_READ(stream.readBytes(&mRenderHeightMap, sizeof(mRenderHeightMap)));
    }
    return {};
}

void SubChunkPacket::SubChunkPacketData::write(BinaryStream& stream, bool cacheEnabled) const {
    mSubChunkPosOffset.write(stream);
    stream.writeEnum(mResult, &BinaryStream::writeByte);
    if (!cacheEnabled || mResult != SubChunkRequestResult::SuccessAllAir) {
        stream.writeString(mSerializedSubChunk);
    }
    mHeightMapData.write(stream);
    if (cacheEnabled) {
        stream.writeUnsignedInt64(mBlobId);
    }
}

Result<> SubChunkPacket::SubChunkPacketData::read(ReadOnlyBinaryStream& stream, bool cacheEnabled) {
    _SCULK_READ(mSubChunkPosOffset.read(stream));
    _SCULK_READ(stream.readEnum(mResult, &ReadOnlyBinaryStream::readByte));
    if (!cacheEnabled || mResult != SubChunkRequestResult::SuccessAllAir) {
        _SCULK_READ(stream.readString(mSerializedSubChunk));
    } else {
        mSerializedSubChunk.clear();
    }
    _SCULK_READ(mHeightMapData.read(stream));
    if (cacheEnabled) {
        return stream.readUnsignedInt64(mBlobId);
    }
    return {};
}

MinecraftPacketIds SubChunkPacket::getId() const noexcept { return MinecraftPacketIds::SubChunk; }

std::string_view SubChunkPacket::getName() const noexcept { return "SubChunkPacket"; }

void SubChunkPacket::write(BinaryStream& stream) const {
    stream.writeBool(mCacheEnabled);
    stream.writeVarInt(mDimensionType);
    mCenterPos.write(stream);
    stream.writeUnsignedInt(static_cast<std::uint32_t>(mSubChunkData.size()));
    for (const SubChunkPacketData& data : mSubChunkData) {
        data.write(stream, mCacheEnabled);
    }
}

Result<> SubChunkPacket::read(ReadOnlyBinaryStream& stream) {
    std::uint32_t count{};
    _SCULK_READ(stream.readBool(mCacheEnabled));
    _SCULK_READ(stream.readVarInt(mDimensionType));
    _SCULK_READ(mCenterPos.read(stream));
    _SCULK_READ(stream.readUnsignedInt(count));
    mSubChunkData.resize(count);
    for (SubChunkPacketData& data : mSubChunkData) {
        _SCULK_READ(data.read(stream, mCacheEnabled));
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SubChunkPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mDimensionType),
        SCULK_FORMAT_FIELD(mSubChunkData),
        SCULK_FORMAT_FIELD(mCenterPos)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
