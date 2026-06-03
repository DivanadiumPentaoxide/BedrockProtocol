// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/NpcRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds NpcRequestPacket::getId() const noexcept { return MinecraftPacketIds::NpcRequest; }

std::string_view NpcRequestPacket::getName() const noexcept { return "NpcRequestPacket"; }

void NpcRequestPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeEnum(mRequestType, &BinaryStream::writeByte);
    stream.writeString(mActions);
    stream.writeByte(mActionIndex);
    stream.writeString(mSceneName);
}

Result<> NpcRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(stream.readEnum(mRequestType, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readString(mActions));
    _SCULK_READ(stream.readByte(mActionIndex));
    return stream.readString(mSceneName);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string NpcRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mRequestType),
        SCULK_FORMAT_FIELD(mActions),
        SCULK_FORMAT_FIELD(mActionIndex),
        SCULK_FORMAT_FIELD(mSceneName)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
