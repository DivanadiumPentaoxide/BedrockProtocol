// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeClimateData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeClimateData::write(BinaryStream& stream) const {
    stream.writeFloat(mTemperature);
    stream.writeFloat(mDownfall);
    stream.writeFloat(mSnowAccumulationMin);
    stream.writeFloat(mSnowAccumulationMax);
}

Result<> BiomeClimateData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readFloat(mTemperature));
    _SCULK_READ(stream.readFloat(mDownfall));
    _SCULK_READ(stream.readFloat(mSnowAccumulationMin));
    return stream.readFloat(mSnowAccumulationMax);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
