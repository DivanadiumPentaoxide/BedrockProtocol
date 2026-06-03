// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MovePlayerPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds MovePlayerPacket::getId() const noexcept { return MinecraftPacketIds::MovePlayer; }
std::string_view   MovePlayerPacket::getName() const noexcept { return "MovePlayerPacket"; }

void MovePlayerPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    mPosition.write(stream);
    mRotation.write(stream);
    stream.writeFloat(mYHeadRotation);
    stream.writeEnum(mPositionMode, &BinaryStream::writeByte);
    stream.writeBool(mOnGround);
    stream.writeUnsignedVarInt64(mRidingRuntimeId);
    if (mPositionMode == PositionMode::Teleport) {
        stream.writeSignedInt(mTeleportationCause);
        stream.writeSignedInt(mSourceActorType);
    }
    stream.writeUnsignedVarInt64(mTick);
}

Result<> MovePlayerPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(mRotation.read(stream));
    _SCULK_READ(stream.readFloat(mYHeadRotation));
    _SCULK_READ(stream.readEnum(mPositionMode, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readBool(mOnGround));
    _SCULK_READ(stream.readUnsignedVarInt64(mRidingRuntimeId));
    if (mPositionMode == PositionMode::Teleport) {
        _SCULK_READ(stream.readSignedInt(mTeleportationCause));
        _SCULK_READ(stream.readSignedInt(mSourceActorType));
    }
    return stream.readUnsignedVarInt64(mTick);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MovePlayerPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mRotation),
        SCULK_FORMAT_FIELD(mYHeadRotation),
        SCULK_FORMAT_FIELD(mPositionMode),
        SCULK_FORMAT_FIELD(mOnGround),
        SCULK_FORMAT_FIELD(mRidingRuntimeId),
        SCULK_FORMAT_FIELD(mTeleportationCause),
        SCULK_FORMAT_FIELD(mSourceActorType),
        SCULK_FORMAT_FIELD(mTick)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
