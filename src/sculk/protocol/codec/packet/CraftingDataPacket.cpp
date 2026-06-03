// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CraftingDataPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CraftingDataPacket::getId() const noexcept { return MinecraftPacketIds::CraftingData; }

std::string_view CraftingDataPacket::getName() const noexcept { return "CraftingDataPacket"; }

void CraftingDataPacket::write(BinaryStream& stream) const {
    stream.writeArray(mCraftingDataEntries, &CraftingDataEntry::write);
    stream.writeArray(mPotionMixDataEntries, &PotionMixDataEntry::write);
    stream.writeArray(mContainerMixDataEntries, &ContainerMixDataEntry::write);
    stream.writeArray(mMaterialReducerDataEntries, &MaterialReducerDataEntry::write);
    stream.writeBool(mClearRecipe);
}

Result<> CraftingDataPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mCraftingDataEntries, &CraftingDataEntry::read));
    _SCULK_READ(stream.readArray(mPotionMixDataEntries, &PotionMixDataEntry::read));
    _SCULK_READ(stream.readArray(mContainerMixDataEntries, &ContainerMixDataEntry::read));
    _SCULK_READ(stream.readArray(mMaterialReducerDataEntries, &MaterialReducerDataEntry::read));
    return stream.readBool(mClearRecipe);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CraftingDataPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mCraftingDataEntries),
        SCULK_FORMAT_FIELD(mPotionMixDataEntries),
        SCULK_FORMAT_FIELD(mContainerMixDataEntries),
        SCULK_FORMAT_FIELD(mMaterialReducerDataEntries)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
