// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/inventory/container/FullContainerName.hpp"
#include "sculk/protocol/codec/inventory/item/NetworkItemInstanceDescriptor.hpp"
#include "sculk/protocol/codec/inventory/recipe/RecipeIngredient.hpp"
#include <string>
#include <variant>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct ItemStackRequestSlotInfo {
    FullContainerName mFullContainerName{};
    std::uint8_t      mSlot{};
    std::int32_t      mNetId{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct ItemStackRequestAction {
    enum class Type : std::uint8_t {
        Take                            = 0,
        Place                           = 1,
        Swap                            = 2,
        Drop                            = 3,
        Destroy                         = 4,
        Consume                         = 5,
        Create                          = 6,
        PlaceInItemContainerDeprecated  = 7,
        TakeFromItemContainerDeprecated = 8,
        LabTableCombine                 = 9,
        BeaconPayment                   = 10,
        MineBlock                       = 11,
        CraftRecipe                     = 12,
        CraftRecipeAuto                 = 13,
        CraftCreative                   = 14,
        CraftRecipeOptional             = 15,
        CraftGrindStone                 = 16,
        CraftLoom                       = 17,
        CraftNonImplemented             = 18,
        CraftResults                    = 19,
    };

    struct TransferBase {
        std::uint8_t             mAmount{};
        ItemStackRequestSlotInfo mSource{};
        ItemStackRequestSlotInfo mDestination{};
    };

    struct Swap {
        ItemStackRequestSlotInfo mSource{};
        ItemStackRequestSlotInfo mDestination{};
    };

    struct Drop {
        std::uint8_t             mAmount{};
        ItemStackRequestSlotInfo mSource{};
        bool                     mRandomly{};
    };

    struct DestroyConsume {
        std::uint8_t             mAmount{};
        ItemStackRequestSlotInfo mSource{};
    };

    struct Create {
        std::uint8_t mResultsIndex{};
    };

    struct BeaconPayment {
        std::int32_t mPrimaryEffectId{};
        std::int32_t mSecondaryEffectId{};
    };

    struct MineBlock {
        enum class PreValidationStatus : std::uint8_t {
            Valid   = 0,
            Invalid = 1,
        };

        std::int32_t        mSlot{};
        std::int32_t        mPredictedDurability{};
        std::int32_t        mItemStackNetId{};
        PreValidationStatus mPreValidationStatus{};
    };

    struct CraftRecipe {
        std::uint32_t mRecipeNetworkIdOrCreativeId{};
        std::uint32_t mTimesCrafted{};
    };

    struct CraftRecipeAuto {
        std::uint32_t                 mRecipeNetworkId{};
        std::uint8_t                  mNumberOfRequestedCrafts{};
        std::uint8_t                  mTimesCrafted{};
        std::vector<RecipeIngredient> mIngredients{};
    };

    struct CraftRecipeOptional {
        std::uint32_t mRecipeNetId{};
        std::uint32_t mFilteredStringIndex{};
    };

    struct CraftGrindStone {
        std::uint32_t mItemStackNetId{};
        std::uint8_t  mTimesCrafted{};
        std::int32_t  mRepairCost{};
    };

    struct CraftLoom {
        std::string  mPatternNameId{};
        std::uint8_t mTimesCrafted{};
    };

    struct CraftResult {
        std::vector<NetworkItemInstanceDescriptor> mCraftResults{};
        std::uint8_t                               mTimesCrafted{};
    };

    struct OnlyType {};

    using Variant = std::variant<
        TransferBase,
        Swap,
        Drop,
        DestroyConsume,
        Create,
        BeaconPayment,
        MineBlock,
        CraftRecipe,
        CraftRecipeAuto,
        CraftRecipeOptional,
        CraftGrindStone,
        CraftLoom,
        CraftResult,
        OnlyType>;

    Type    mActionType{};
    Variant mVariant{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct ItemStackRequestData {
    std::int32_t                        mClientRequestId{};
    std::vector<std::string>            mStringsToFilter{};
    std::int32_t                        mStringsToFilterOrigin{};
    std::vector<ItemStackRequestAction> mActions{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct ItemStackRequest {
    std::vector<ItemStackRequestData> mRequests{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
