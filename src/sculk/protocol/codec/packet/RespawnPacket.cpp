// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/RespawnPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds RespawnPacket::getId() const noexcept { return MinecraftPacketIds::Respawn; }

std::string_view RespawnPacket::getName() const noexcept { return "RespawnPacket"; }

void RespawnPacket::write(BinaryStream& stream) const {
    mPosition.write(stream);
    stream.writeEnum(mState, &BinaryStream::writeByte);
    stream.writeUnsignedVarInt64(mActorRuntimeId);
}

Result<> RespawnPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readEnum(mState, &ReadOnlyBinaryStream::readByte));
    return stream.readUnsignedVarInt64(mActorRuntimeId);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string RespawnPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mState),
        SCULK_FORMAT_FIELD(mActorRuntimeId)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
