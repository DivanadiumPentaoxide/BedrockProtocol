// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/DebugInfoPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds DebugInfoPacket::getId() const noexcept { return MinecraftPacketIds::DebugInfo; }

std::string_view DebugInfoPacket::getName() const noexcept { return "DebugInfoPacket"; }

void DebugInfoPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mActorUniqueId);
    stream.writeString(mData);
}

Result<> DebugInfoPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mActorUniqueId));
    return stream.readString(mData);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string DebugInfoPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mActorUniqueId), SCULK_FORMAT_FIELD(mData));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
