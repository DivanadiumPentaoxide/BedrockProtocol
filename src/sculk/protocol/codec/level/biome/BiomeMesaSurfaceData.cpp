// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeMesaSurfaceData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeMesaSurfaceData::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(mClayMaterial);
    stream.writeUnsignedInt(mHardClayMaterial);
    stream.writeBool(mBrycePillars);
    stream.writeBool(mHasForest);
}

Result<> BiomeMesaSurfaceData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt(mClayMaterial));
    _SCULK_READ(stream.readUnsignedInt(mHardClayMaterial));
    _SCULK_READ(stream.readBool(mBrycePillars));
    return stream.readBool(mHasForest);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
