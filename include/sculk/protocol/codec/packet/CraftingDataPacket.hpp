// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/inventory/recipe/Recipes.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class CraftingDataPacket : public IPacket {
public:
    std::vector<CraftingDataEntry>        mCraftingDataEntries{};
    std::vector<PotionMixDataEntry>       mPotionMixDataEntries{};
    std::vector<ContainerMixDataEntry>    mContainerMixDataEntries{};
    std::vector<MaterialReducerDataEntry> mMaterialReducerDataEntries{};
    bool                                  mClearRecipe{false};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(CraftingDataPacket)