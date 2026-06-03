// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/UnlockedRecipesPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds UnlockedRecipesPacket::getId() const noexcept { return MinecraftPacketIds::UnlockedRecipes; }

std::string_view UnlockedRecipesPacket::getName() const noexcept { return "UnlockedRecipesPacket"; }

void UnlockedRecipesPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mPacketType, &BinaryStream::writeUnsignedInt);
    stream.writeArray(mUnlockedRecipesList, &BinaryStream::writeString);
}

Result<> UnlockedRecipesPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mPacketType, &ReadOnlyBinaryStream::readUnsignedInt));
    return stream.readArray(mUnlockedRecipesList, &ReadOnlyBinaryStream::readString);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string UnlockedRecipesPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mPacketType), SCULK_FORMAT_FIELD(mUnlockedRecipesList));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
