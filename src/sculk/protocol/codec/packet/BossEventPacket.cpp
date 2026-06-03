// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/BossEventPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds BossEventPacket::getId() const noexcept { return MinecraftPacketIds::BossEvent; }

std::string_view BossEventPacket::getName() const noexcept { return "BossEventPacket"; }

void BossEventPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mTargetActor);
    stream.writeEnum(mType, &BinaryStream::writeUnsignedVarInt);
    switch (mType) {
    case EventType::Add:
        stream.writeString(mName);
        stream.writeString(mFilteredName);
        stream.writeFloat(mPercentage);
        stream.writeUnsignedShort(mDarkenScreen);
        stream.writeUnsignedVarInt(mColor);
        stream.writeUnsignedVarInt(mOverlay);
        break;
    case EventType::PlayerAdded:
    case EventType::PlayerRemoved:
    case EventType::Query:
        stream.writeVarInt64(mPlayer);
        break;
    case EventType::UpdatePercent:
        stream.writeFloat(mPercentage);
        break;
    case EventType::UpdateName:
        stream.writeString(mName);
        stream.writeString(mFilteredName);
        break;
    case EventType::UpdateProperties:
        stream.writeUnsignedShort(mDarkenScreen);
        stream.writeUnsignedVarInt(mColor);
        stream.writeUnsignedVarInt(mOverlay);
        break;
    case EventType::UpdateStyle:
        stream.writeUnsignedVarInt(mColor);
        stream.writeUnsignedVarInt(mOverlay);
        break;
    default:
        break;
    }
}

Result<> BossEventPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mTargetActor));
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readUnsignedVarInt));
    switch (mType) {
    case EventType::Add:
        _SCULK_READ(stream.readString(mName));
        _SCULK_READ(stream.readString(mFilteredName));
        _SCULK_READ(stream.readFloat(mPercentage));
        _SCULK_READ(stream.readUnsignedShort(mDarkenScreen));
        _SCULK_READ(stream.readUnsignedVarInt(mColor));
        return stream.readUnsignedVarInt(mOverlay);
    case EventType::PlayerAdded:
    case EventType::PlayerRemoved:
    case EventType::Query:
        return stream.readVarInt64(mPlayer);
    case EventType::UpdatePercent:
        return stream.readFloat(mPercentage);
    case EventType::UpdateName:
        _SCULK_READ(stream.readString(mName));
        return stream.readString(mFilteredName);
    case EventType::UpdateProperties:
        _SCULK_READ(stream.readUnsignedShort(mDarkenScreen));
        _SCULK_READ(stream.readUnsignedVarInt(mColor));
        return stream.readUnsignedVarInt(mOverlay);
    case EventType::UpdateStyle:
        _SCULK_READ(stream.readUnsignedVarInt(mColor));
        return stream.readUnsignedVarInt(mOverlay);
    default:
        return {};
    }
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string BossEventPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mTargetActor),
        SCULK_FORMAT_FIELD(mPlayer),
        SCULK_FORMAT_FIELD(mType),
        SCULK_FORMAT_FIELD(mName),
        SCULK_FORMAT_FIELD(mFilteredName),
        SCULK_FORMAT_FIELD(mPercentage),
        SCULK_FORMAT_FIELD(mDarkenScreen),
        SCULK_FORMAT_FIELD(mColor),
        SCULK_FORMAT_FIELD(mOverlay)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
