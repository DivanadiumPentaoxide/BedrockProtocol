// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeCoordinateData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeCoordinateData::write(BinaryStream& stream) const {
    stream.writeVarInt(mMinValueType);
    stream.writeUnsignedShort(mMinValue);
    stream.writeVarInt(mMaxValueType);
    stream.writeUnsignedShort(mMaxValue);
    stream.writeUnsignedInt(mGridOffset);
    stream.writeUnsignedInt(mGridStepSize);
    stream.writeEnum(mDistributionType, &BinaryStream::writeVarInt);
}

Result<> BiomeCoordinateData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mMinValueType));
    _SCULK_READ(stream.readUnsignedShort(mMinValue));
    _SCULK_READ(stream.readVarInt(mMaxValueType));
    _SCULK_READ(stream.readUnsignedShort(mMaxValue));
    _SCULK_READ(stream.readUnsignedInt(mGridOffset));
    _SCULK_READ(stream.readUnsignedInt(mGridStepSize));
    return stream.readEnum(mDistributionType, &ReadOnlyBinaryStream::readVarInt);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
