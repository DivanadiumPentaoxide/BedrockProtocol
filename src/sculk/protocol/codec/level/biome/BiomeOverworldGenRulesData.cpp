// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeOverworldGenRulesData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeOverworldGenRulesData::write(BinaryStream& stream) const {
    stream.writeArray(mHillsTransformations, &BiomeWeightedData::write);
    stream.writeArray(mMutateTransformations, &BiomeWeightedData::write);
    stream.writeArray(mRiverTransformations, &BiomeWeightedData::write);
    stream.writeArray(mShoreTransformations, &BiomeWeightedData::write);
    stream.writeArray(mPreHillsEdge, &BiomeConditionalTransformationData::write);
    stream.writeArray(mPreShoreEdge, &BiomeConditionalTransformationData::write);
    stream.writeArray(mClimate, &BiomeWeightedTemperatureData::write);
}
Result<> BiomeOverworldGenRulesData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mHillsTransformations, &BiomeWeightedData::read));
    _SCULK_READ(stream.readArray(mMutateTransformations, &BiomeWeightedData::read));
    _SCULK_READ(stream.readArray(mRiverTransformations, &BiomeWeightedData::read));
    _SCULK_READ(stream.readArray(mShoreTransformations, &BiomeWeightedData::read));
    _SCULK_READ(stream.readArray(mPreHillsEdge, &BiomeConditionalTransformationData::read));
    _SCULK_READ(stream.readArray(mPreShoreEdge, &BiomeConditionalTransformationData::read));
    return stream.readArray(mClimate, &BiomeWeightedTemperatureData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
