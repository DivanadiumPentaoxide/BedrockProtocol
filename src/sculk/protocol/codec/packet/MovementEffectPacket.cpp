// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MovementEffectPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds MovementEffectPacket::getId() const noexcept { return MinecraftPacketIds::MovementEffect; }

std::string_view MovementEffectPacket::getName() const noexcept { return "MovementEffectPacket"; }

void MovementEffectPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeEnum(mEffectId, &BinaryStream::writeVarInt);
    stream.writeVarInt(mEffectDuration);
    stream.writeUnsignedVarInt64(mTick);
}

Result<> MovementEffectPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(stream.readEnum(mEffectId, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readVarInt(mEffectDuration));
    return stream.readUnsignedVarInt64(mTick);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MovementEffectPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mEffectId),
        SCULK_FORMAT_FIELD(mEffectDuration),
        SCULK_FORMAT_FIELD(mTick)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
