// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/DimensionDataPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void DimensionDataPacket::DimensionDefinition::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeVarInt(mHeightMax);
    stream.writeVarInt(mHeightMin);
    stream.writeEnum(mGeneratorType, &BinaryStream::writeVarInt);
    stream.writeVarInt(mDimensionType);
}

Result<> DimensionDataPacket::DimensionDefinition::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readVarInt(mHeightMax));
    _SCULK_READ(stream.readVarInt(mHeightMin));
    _SCULK_READ(stream.readEnum(mGeneratorType, &ReadOnlyBinaryStream::readVarInt));
    return stream.readVarInt(mDimensionType);
}

MinecraftPacketIds DimensionDataPacket::getId() const noexcept { return MinecraftPacketIds::DimensionData; }

std::string_view DimensionDataPacket::getName() const noexcept { return "DimensionDataPacket"; }

void DimensionDataPacket::write(BinaryStream& stream) const {
    stream.writeArray(mDefinitionGroup, &DimensionDefinition::write);
}

Result<> DimensionDataPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mDefinitionGroup, &DimensionDefinition::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string DimensionDataPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mDefinitionGroup)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
