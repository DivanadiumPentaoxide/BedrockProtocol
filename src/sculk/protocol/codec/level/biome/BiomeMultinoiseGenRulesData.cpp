// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeMultinoiseGenRulesData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeMultinoiseGenRulesData::write(BinaryStream& stream) const {
    stream.writeFloat(mTemperature);
    stream.writeFloat(mHumidity);
    stream.writeFloat(mAltitude);
    stream.writeFloat(mWeirdness);
    stream.writeFloat(mWeight);
}
Result<> BiomeMultinoiseGenRulesData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readFloat(mTemperature));
    _SCULK_READ(stream.readFloat(mHumidity));
    _SCULK_READ(stream.readFloat(mAltitude));
    _SCULK_READ(stream.readFloat(mWeirdness));
    return stream.readFloat(mWeight);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
