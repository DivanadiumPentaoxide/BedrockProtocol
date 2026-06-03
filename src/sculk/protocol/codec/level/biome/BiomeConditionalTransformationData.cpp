// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeConditionalTransformationData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeConditionalTransformationData::write(BinaryStream& stream) const {
    stream.writeArray(mTransformsInto, &BiomeWeightedData::write);
    stream.writeSignedShort(mConditionJSON);
    stream.writeUnsignedInt(mMinPassingNeighbors);
}

Result<> BiomeConditionalTransformationData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mTransformsInto, &BiomeWeightedData::read));
    _SCULK_READ(stream.readSignedShort(mConditionJSON));
    return stream.readUnsignedInt(mMinPassingNeighbors);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
