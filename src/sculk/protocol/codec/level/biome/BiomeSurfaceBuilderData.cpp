// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeSurfaceBuilderData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeSurfaceBuilderData::write(BinaryStream& stream) const {
    stream.writeOptional(mSurfaceMaterials, &BiomeSurfaceMaterialData::write);
    stream.writeBool(mHasDefaultOverworldSurface);
    stream.writeBool(mHasSwampSurface);
    stream.writeBool(mHasFrozenOceanSurface);
    stream.writeBool(mHasTheEndSurface);
    stream.writeOptional(mMesaSurface, &BiomeMesaSurfaceData::write);
    stream.writeOptional(mCappedSurface, &BiomeCappedSurfaceData::write);
    stream.writeOptional(mNoiseGradientSurface, &BiomeNoiseGradientSurfaceData::write);
}

Result<> BiomeSurfaceBuilderData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readOptional(mSurfaceMaterials, &BiomeSurfaceMaterialData::read));
    _SCULK_READ(stream.readBool(mHasDefaultOverworldSurface));
    _SCULK_READ(stream.readBool(mHasSwampSurface));
    _SCULK_READ(stream.readBool(mHasFrozenOceanSurface));
    _SCULK_READ(stream.readBool(mHasTheEndSurface));
    _SCULK_READ(stream.readOptional(mMesaSurface, &BiomeMesaSurfaceData::read));
    _SCULK_READ(stream.readOptional(mCappedSurface, &BiomeCappedSurfaceData::read));
    return stream.readOptional(mNoiseGradientSurface, &BiomeNoiseGradientSurfaceData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE