// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/RemoveVolumeEntityPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds RemoveVolumeEntityPacket::getId() const noexcept { return MinecraftPacketIds::RemoveVolumeEntity; }

std::string_view RemoveVolumeEntityPacket::getName() const noexcept { return "RemoveVolumeEntityPacket"; }

void RemoveVolumeEntityPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(mEntityNetId);
    stream.writeVarInt(mDimensionType);
}

Result<> RemoveVolumeEntityPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt(mEntityNetId));
    return stream.readVarInt(mDimensionType);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string RemoveVolumeEntityPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mEntityNetId), SCULK_FORMAT_FIELD(mDimensionType));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
