// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MoveActorAbsolutePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds MoveActorAbsolutePacket::getId() const noexcept { return MinecraftPacketIds::MoveActorAbsolute; }

std::string_view MoveActorAbsolutePacket::getName() const noexcept { return "MoveActorAbsolutePacket"; }

void MoveActorAbsolutePacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeEnum(mFlags, &BinaryStream::writeByte);
    mPosition.write(stream);
    stream.writeByte(mRotationX);
    stream.writeByte(mRotationY);
    stream.writeByte(mRotationYHead);
}

Result<> MoveActorAbsolutePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(stream.readEnum(mFlags, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readByte(mRotationX));
    _SCULK_READ(stream.readByte(mRotationY));
    return stream.readByte(mRotationYHead);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MoveActorAbsolutePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mFlags),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mRotationX),
        SCULK_FORMAT_FIELD(mRotationY),
        SCULK_FORMAT_FIELD(mRotationYHead),
        SCULK_FORMAT_FIELD(mForceCompletion)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
