// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeDefinitionData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeDefinitionData::write(BinaryStream& stream) const {
    stream.writeUnsignedShort(mBiomeId);
    stream.writeFloat(mTemperature);
    stream.writeFloat(mDownfall);
    stream.writeFloat(mFoliageSnow);
    stream.writeFloat(mDepth);
    stream.writeFloat(mScale);
    stream.writeSignedInt(mMapWaterColorARGB);
    stream.writeBool(mRain);
    stream.writeOptional(mTags, &BiomeTagsData::write);
    stream.writeOptional(mChunkGenData, &BiomeDefinitionChunkGenData::write);
}

Result<> BiomeDefinitionData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedShort(mBiomeId));
    _SCULK_READ(stream.readFloat(mTemperature));
    _SCULK_READ(stream.readFloat(mDownfall));
    _SCULK_READ(stream.readFloat(mFoliageSnow));
    _SCULK_READ(stream.readFloat(mDepth));
    _SCULK_READ(stream.readFloat(mScale));
    _SCULK_READ(stream.readSignedInt(mMapWaterColorARGB));
    _SCULK_READ(stream.readBool(mRain));
    _SCULK_READ(stream.readOptional(mTags, &BiomeTagsData::read));
    return stream.readOptional(mChunkGenData, &BiomeDefinitionChunkGenData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE