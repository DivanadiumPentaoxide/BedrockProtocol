// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeDefinitionChunkGenData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeDefinitionChunkGenData::write(BinaryStream& stream) const {
    stream.writeOptional(mClimate, &BiomeClimateData::write);
    stream.writeOptional(mConsolidatedFeatures, &BiomeConsolidatedFeaturesData::write);
    stream.writeOptional(mMountainParams, &BiomeMountainParamsData::write);
    stream.writeOptional(mSurfaceMaterialAdjustments, &BiomeSurfaceMaterialAdjustmentData::write);
    stream.writeOptional(mOverworldGenRules, &BiomeOverworldGenRulesData::write);
    stream.writeOptional(mMultinoiseGenRules, &BiomeMultinoiseGenRulesData::write);
    stream.writeOptional(mLegacyWorldGenRules, &BiomeLegacyWorldGenRulesData::write);
    stream.writeOptional(mBiomeReplacementData, &BiomeReplacementData::write);
    stream.writeOptional(mVillageType, [](BinaryStream& stream, const VillageType& value) {
        stream.writeEnum(value, &BinaryStream::writeByte);
    });
    stream.writeOptional(mSurfaceBuilderData, &BiomeSurfaceBuilderData::write);
    stream.writeOptional(mSubSurfaceBuilderData, &BiomeSurfaceBuilderData::write);
}

Result<> BiomeDefinitionChunkGenData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readOptional(mClimate, &BiomeClimateData::read));
    _SCULK_READ(stream.readOptional(mConsolidatedFeatures, &BiomeConsolidatedFeaturesData::read));
    _SCULK_READ(stream.readOptional(mMountainParams, &BiomeMountainParamsData::read));
    _SCULK_READ(stream.readOptional(mSurfaceMaterialAdjustments, &BiomeSurfaceMaterialAdjustmentData::read));
    _SCULK_READ(stream.readOptional(mOverworldGenRules, &BiomeOverworldGenRulesData::read));
    _SCULK_READ(stream.readOptional(mMultinoiseGenRules, &BiomeMultinoiseGenRulesData::read));
    _SCULK_READ(stream.readOptional(mLegacyWorldGenRules, &BiomeLegacyWorldGenRulesData::read));
    _SCULK_READ(stream.readOptional(mBiomeReplacementData, &BiomeReplacementData::read));
    _SCULK_READ(stream.readOptional(mVillageType, [](ReadOnlyBinaryStream& stream, VillageType& value) {
        return stream.readEnum(value, &ReadOnlyBinaryStream::readByte);
    }));
    _SCULK_READ(stream.readOptional(mSurfaceBuilderData, &BiomeSurfaceBuilderData::read));
    return stream.readOptional(mSubSurfaceBuilderData, &BiomeSurfaceBuilderData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE