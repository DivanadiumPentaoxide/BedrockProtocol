// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/TakeItemActorPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds TakeItemActorPacket::getId() const noexcept { return MinecraftPacketIds::TakeItemActor; }

std::string_view TakeItemActorPacket::getName() const noexcept { return "TakeItemActorPacket"; }

void TakeItemActorPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mItemRuntimeId);
    stream.writeUnsignedVarInt64(mActorRuntimeId);
}

Result<> TakeItemActorPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mItemRuntimeId));
    return stream.readUnsignedVarInt64(mActorRuntimeId);
}


#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string TakeItemActorPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mItemRuntimeId), SCULK_FORMAT_FIELD(mActorRuntimeId));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
