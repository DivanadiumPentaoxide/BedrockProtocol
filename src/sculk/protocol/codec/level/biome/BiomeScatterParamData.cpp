// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeScatterParamData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeScatterParamData::write(BinaryStream& stream) const {
    stream.writeArray(mCoordinate, &BiomeCoordinateData::write);
    stream.writeEnum(mEvalOrder, &BinaryStream::writeVarInt);
    stream.writeVarInt(mChancePercentType);
    stream.writeUnsignedShort(mChancePercent);
    stream.writeSignedInt(mChanceNumeartor);
    stream.writeSignedInt(mChanceDenominator);
    stream.writeVarInt(mIterationsType);
    stream.writeUnsignedShort(mIterations);
}

Result<> BiomeScatterParamData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mCoordinate, &BiomeCoordinateData::read));
    _SCULK_READ(stream.readEnum(mEvalOrder, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readVarInt(mChancePercentType));
    _SCULK_READ(stream.readUnsignedShort(mChancePercent));
    _SCULK_READ(stream.readSignedInt(mChanceNumeartor));
    _SCULK_READ(stream.readSignedInt(mChanceDenominator));
    _SCULK_READ(stream.readVarInt(mIterationsType));
    return stream.readUnsignedShort(mIterations);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
