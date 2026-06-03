// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeMountainParamsData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeMountainParamsData::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(mSteepBlock);
    stream.writeBool(mNorthSlopes);
    stream.writeBool(mSouthSlopes);
    stream.writeBool(mWestSlopes);
    stream.writeBool(mEastSlopes);
    stream.writeBool(mTopSlideEnabled);
}

Result<> BiomeMountainParamsData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt(mSteepBlock));
    _SCULK_READ(stream.readBool(mNorthSlopes));
    _SCULK_READ(stream.readBool(mSouthSlopes));
    _SCULK_READ(stream.readBool(mWestSlopes));
    _SCULK_READ(stream.readBool(mEastSlopes));
    return stream.readBool(mTopSlideEnabled);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
