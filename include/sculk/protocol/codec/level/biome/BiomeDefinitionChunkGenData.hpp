// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "BiomeCappedSurfaceData.hpp"
#include "BiomeClimateData.hpp"
#include "BiomeConsolidatedFeaturesData.hpp"
#include "BiomeLegacyWorldGenRulesData.hpp"
#include "BiomeMesaSurfaceData.hpp"
#include "BiomeMountainParamsData.hpp"
#include "BiomeMultinoiseGenRulesData.hpp"
#include "BiomeOverworldGenRulesData.hpp"
#include "BiomeReplacementData.hpp"
#include "BiomeSurfaceBuilderData.hpp"
#include "BiomeSurfaceMaterialAdjustmentData.hpp"
#include "BiomeSurfaceMaterialData.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct BiomeDefinitionChunkGenData {
    enum class VillageType : std::uint8_t {
        Desert  = 0,
        Ice     = 1,
        Savanna = 2,
        Taiga   = 3,
        Default = 4,
    };

    std::optional<BiomeClimateData>                   mClimate{};
    std::optional<BiomeConsolidatedFeaturesData>      mConsolidatedFeatures{};
    std::optional<BiomeMountainParamsData>            mMountainParams{};
    std::optional<BiomeSurfaceMaterialAdjustmentData> mSurfaceMaterialAdjustments{};
    std::optional<BiomeOverworldGenRulesData>         mOverworldGenRules{};
    std::optional<BiomeMultinoiseGenRulesData>        mMultinoiseGenRules{};
    std::optional<BiomeLegacyWorldGenRulesData>       mLegacyWorldGenRules{};
    std::optional<BiomeReplacementData>               mBiomeReplacementData{};
    std::optional<VillageType>                        mVillageType{};
    std::optional<BiomeSurfaceBuilderData>            mSurfaceBuilderData{};
    std::optional<BiomeSurfaceBuilderData>            mSubSurfaceBuilderData{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
