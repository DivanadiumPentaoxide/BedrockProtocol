// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeReplacementData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeReplacementData::write(BinaryStream& stream) const {
    stream.writeSignedShort(mBiome);
    stream.writeSignedShort(mDimension);
    stream.writeArray(mTargetBiomes, &BinaryStream::writeSignedShort);
    stream.writeFloat(mAmount);
    stream.writeFloat(mNoiseFrequencyScale);
    stream.writeUnsignedInt(mReplacementIndex);
}
Result<> BiomeReplacementData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedShort(mBiome));
    _SCULK_READ(stream.readSignedShort(mDimension));
    _SCULK_READ(stream.readArray(mTargetBiomes, &ReadOnlyBinaryStream::readSignedShort));
    _SCULK_READ(stream.readFloat(mAmount));
    _SCULK_READ(stream.readFloat(mNoiseFrequencyScale));
    return stream.readUnsignedInt(mReplacementIndex);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
