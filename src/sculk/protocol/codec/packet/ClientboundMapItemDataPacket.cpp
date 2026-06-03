// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientboundMapItemDataPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientboundMapItemDataPacket::getId() const noexcept {
    return MinecraftPacketIds::ClientboundMapItemData;
}

std::string_view ClientboundMapItemDataPacket::getName() const noexcept { return "ClientboundMapItemDataPacket"; }

void ClientboundMapItemDataPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mMapId);
    stream.writeEnum(mTypeFlag, &BinaryStream::writeUnsignedVarInt);
    stream.writeByte(mDimension);
    stream.writeBool(mIsLockedMap);
    mMapOrigin.write(stream);
    if (static_cast<bool>(mTypeFlag & ClientboundMapItemDataType::Creation)) {
        stream.writeArray(mMapEntries, &BinaryStream::writeVarInt64);
    }
    if (static_cast<bool>(mTypeFlag & ClientboundMapItemDataType::All)) {
        stream.writeByte(mScale);
    }
    if (static_cast<bool>(mTypeFlag & ClientboundMapItemDataType::DecorationUpdate)) {
        stream.writeArray(mTrackedActors, &MapTrackedActorUniqueId::write);
        stream.writeArray(mDecorationList, &MapDecoration::write);
    }
    if (static_cast<bool>(mTypeFlag & ClientboundMapItemDataType::TextureUpdate)) {
        stream.writeVarInt(mTextureWidth);
        stream.writeVarInt(mTextureHeight);
        stream.writeVarInt(mXTexCoordinate);
        stream.writeVarInt(mYTexCoordinate);
        stream.writeArray(mPixels, &BinaryStream::writeUnsignedVarInt);
    }
}

Result<> ClientboundMapItemDataPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mMapId));
    _SCULK_READ(stream.readEnum(mTypeFlag, &ReadOnlyBinaryStream::readUnsignedVarInt));
    _SCULK_READ(stream.readByte(mDimension));
    _SCULK_READ(stream.readBool(mIsLockedMap));
    _SCULK_READ(mMapOrigin.read(stream));
    if (static_cast<bool>(mTypeFlag & ClientboundMapItemDataType::Creation)) {
        _SCULK_READ(stream.readArray(mMapEntries, &ReadOnlyBinaryStream::readVarInt64));
    }
    if (static_cast<bool>(mTypeFlag & ClientboundMapItemDataType::All)) {
        _SCULK_READ(stream.readByte(mScale));
    }
    if (static_cast<bool>(mTypeFlag & ClientboundMapItemDataType::DecorationUpdate)) {
        _SCULK_READ(stream.readArray(mTrackedActors, &MapTrackedActorUniqueId::read));
        _SCULK_READ(stream.readArray(mDecorationList, &MapDecoration::read));
    }
    if (static_cast<bool>(mTypeFlag & ClientboundMapItemDataType::TextureUpdate)) {
        _SCULK_READ(stream.readVarInt(mTextureWidth));
        _SCULK_READ(stream.readVarInt(mTextureHeight));
        _SCULK_READ(stream.readVarInt(mXTexCoordinate));
        _SCULK_READ(stream.readVarInt(mYTexCoordinate));
        _SCULK_READ(stream.readArray(mPixels, &ReadOnlyBinaryStream::readUnsignedVarInt));
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientboundMapItemDataPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mMapId),
        SCULK_FORMAT_FIELD(mTypeFlag),
        SCULK_FORMAT_FIELD(mDimension),
        SCULK_FORMAT_FIELD(mIsLockedMap),
        SCULK_FORMAT_FIELD(mMapOrigin),
        SCULK_FORMAT_FIELD(mMapEntries),
        SCULK_FORMAT_FIELD(mScale),
        SCULK_FORMAT_FIELD(mTrackedActors),
        SCULK_FORMAT_FIELD(mDecorationList),
        SCULK_FORMAT_FIELD(mTextureWidth),
        SCULK_FORMAT_FIELD(mTextureHeight),
        SCULK_FORMAT_FIELD(mXTexCoordinate),
        SCULK_FORMAT_FIELD(mYTexCoordinate),
        SCULK_FORMAT_FIELD(mPixels)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
