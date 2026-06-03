// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/BiomeDefinitionListPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds BiomeDefinitionListPacket::getId() const noexcept { return MinecraftPacketIds::BiomeDefinitionList; }

std::string_view BiomeDefinitionListPacket::getName() const noexcept { return "BiomeDefinitionListPacket"; }

void BiomeDefinitionListPacket::write(BinaryStream& stream) const {
    stream.writeArray(mBiomeNamesToDataMap, &BiomeData::write);
    stream.writeArray(mBiomeStringList, &BinaryStream::writeString);
}

Result<> BiomeDefinitionListPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mBiomeNamesToDataMap, &BiomeData::read));
    return stream.readArray(mBiomeStringList, &ReadOnlyBinaryStream::readString);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string BiomeDefinitionListPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mBiomeNamesToDataMap), SCULK_FORMAT_FIELD(mBiomeStringList));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
