// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeElementData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeElementData::write(BinaryStream& stream) const {
    stream.writeFloat(mNoiseFrequencyScale);
    stream.writeFloat(mNoiseLowerBound);
    stream.writeFloat(mNoiseUpperBound);
    stream.writeVarInt(mHeightMinType);
    stream.writeSignedShort(mHeightMin);
    stream.writeVarInt(mHeightMaxType);
    stream.writeSignedShort(mHeightMax);
    mAdjustedMaterials.write(stream);
}

Result<> BiomeElementData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readFloat(mNoiseFrequencyScale));
    _SCULK_READ(stream.readFloat(mNoiseLowerBound));
    _SCULK_READ(stream.readFloat(mNoiseUpperBound));
    _SCULK_READ(stream.readVarInt(mHeightMinType));
    _SCULK_READ(stream.readSignedShort(mHeightMin));
    _SCULK_READ(stream.readVarInt(mHeightMaxType));
    _SCULK_READ(stream.readSignedShort(mHeightMax));
    return mAdjustedMaterials.read(stream);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
