// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MapCreateLockedCopyPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds MapCreateLockedCopyPacket::getId() const noexcept { return MinecraftPacketIds::MapCreateLockedCopy; }

std::string_view MapCreateLockedCopyPacket::getName() const noexcept { return "MapCreateLockedCopyPacket"; }

void MapCreateLockedCopyPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mOriginMapId);
    stream.writeVarInt64(mNewMapId);
}

Result<> MapCreateLockedCopyPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mOriginMapId));
    return stream.readVarInt64(mNewMapId);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MapCreateLockedCopyPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mOriginMapId), SCULK_FORMAT_FIELD(mNewMapId));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
