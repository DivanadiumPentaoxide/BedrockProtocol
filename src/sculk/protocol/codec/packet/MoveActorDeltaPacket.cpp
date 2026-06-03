// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MoveActorDeltaPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace {

constexpr std::uint16_t HasX       = 1u << 0;
constexpr std::uint16_t HasY       = 1u << 1;
constexpr std::uint16_t HasZ       = 1u << 2;
constexpr std::uint16_t HasPitch   = 1u << 3;
constexpr std::uint16_t HasYaw     = 1u << 4;
constexpr std::uint16_t HasHeadYaw = 1u << 5;

} // namespace

MinecraftPacketIds MoveActorDeltaPacket::getId() const noexcept { return MinecraftPacketIds::MoveActorDelta; }

std::string_view MoveActorDeltaPacket::getName() const noexcept { return "MoveActorDeltaPacket"; }

void MoveActorDeltaPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeUnsignedShort(mHeader);
    if ((mHeader & HasX) != 0) {
        stream.writeFloat(mNewPosX);
    }
    if ((mHeader & HasY) != 0) {
        stream.writeFloat(mNewPosY);
    }
    if ((mHeader & HasZ) != 0) {
        stream.writeFloat(mNewPosZ);
    }
    if ((mHeader & HasPitch) != 0) {
        stream.writeByte(mRotationXByteAngle);
    }
    if ((mHeader & HasYaw) != 0) {
        stream.writeByte(mRotationYByteAngle);
    }
    if ((mHeader & HasHeadYaw) != 0) {
        stream.writeByte(mRotationYHeaderByteAngle);
    }
}

Result<> MoveActorDeltaPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(stream.readUnsignedShort(mHeader));
    if ((mHeader & HasX) != 0) {
        _SCULK_READ(stream.readFloat(mNewPosX));
    }
    if ((mHeader & HasY) != 0) {
        _SCULK_READ(stream.readFloat(mNewPosY));
    }
    if ((mHeader & HasZ) != 0) {
        _SCULK_READ(stream.readFloat(mNewPosZ));
    }
    if ((mHeader & HasPitch) != 0) {
        _SCULK_READ(stream.readByte(mRotationXByteAngle));
    }
    if ((mHeader & HasYaw) != 0) {
        _SCULK_READ(stream.readByte(mRotationYByteAngle));
    }
    if ((mHeader & HasHeadYaw) != 0) {
        _SCULK_READ(stream.readByte(mRotationYHeaderByteAngle));
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MoveActorDeltaPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mHeader),
        SCULK_FORMAT_FIELD(mNewPosX),
        SCULK_FORMAT_FIELD(mNewPosY),
        SCULK_FORMAT_FIELD(mNewPosZ),
        SCULK_FORMAT_FIELD(mRotationXByteAngle),
        SCULK_FORMAT_FIELD(mRotationYByteAngle),
        SCULK_FORMAT_FIELD(mRotationYHeaderByteAngle)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
