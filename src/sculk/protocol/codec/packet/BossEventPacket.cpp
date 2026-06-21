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
    stream.writeVarInt64(mTargetActorID);
    stream.writeVarInt64(mPlayerID);
    stream.writeEnum(mType, &BinaryStream::writeUnsignedVarInt);
    stream.writeString(mName);
    stream.writeString(mFilteredName);
    stream.writeFloat(mPercentage);
    stream.writeUnsignedVarInt(mColor);
    stream.writeUnsignedVarInt(mOverlay);
}

Result<> BossEventPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mTargetActorID));
    _SCULK_READ(stream.readVarInt64(mPlayerID));
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readUnsignedVarInt));
    _SCULK_READ(stream.readString(mName));
    _SCULK_CHECK_MAX(mName.size(), 256)
    _SCULK_READ(stream.readString(mFilteredName));
    _SCULK_CHECK_MAX(mFilteredName.size(), 256)
    _SCULK_READ(stream.readFloat(mPercentage));
    _SCULK_READ(stream.readUnsignedVarInt(mColor));
    return stream.readUnsignedVarInt(mOverlay);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string BossEventPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mTargetActorID),
        SCULK_FORMAT_FIELD(mPlayerID),
        SCULK_FORMAT_FIELD(mType),
        SCULK_FORMAT_FIELD(mName),
        SCULK_FORMAT_FIELD(mFilteredName),
        SCULK_FORMAT_FIELD(mPercentage),
        SCULK_FORMAT_FIELD(mColor),
        SCULK_FORMAT_FIELD(mOverlay)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
