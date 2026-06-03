// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AddPaintingPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AddPaintingPacket::getId() const noexcept { return MinecraftPacketIds::AddPainting; }

std::string_view AddPaintingPacket::getName() const noexcept { return "AddPaintingPacket"; }

void AddPaintingPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mUniqueId);
    stream.writeUnsignedVarInt64(mRuntimeId);
    mPosition.write(stream);
    stream.writeVarInt(mDirection);
    stream.writeString(mMotif);
}

Result<> AddPaintingPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mUniqueId));
    _SCULK_READ(stream.readUnsignedVarInt64(mRuntimeId));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readVarInt(mDirection));
    return stream.readString(mMotif);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AddPaintingPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mRuntimeId),
        SCULK_FORMAT_FIELD(mUniqueId),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mDirection),
        SCULK_FORMAT_FIELD(mMotif)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
