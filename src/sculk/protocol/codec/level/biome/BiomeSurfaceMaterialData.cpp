// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeSurfaceMaterialData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeSurfaceMaterialData::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(mTopBlock);
    stream.writeUnsignedInt(mMidBlock);
    stream.writeUnsignedInt(mSeaFloorBlock);
    stream.writeUnsignedInt(mFoundationBlock);
    stream.writeUnsignedInt(mSeaBlock);
    stream.writeSignedInt(mSeaFloorDepth);
}

Result<> BiomeSurfaceMaterialData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt(mTopBlock));
    _SCULK_READ(stream.readUnsignedInt(mMidBlock));
    _SCULK_READ(stream.readUnsignedInt(mSeaFloorBlock));
    _SCULK_READ(stream.readUnsignedInt(mFoundationBlock));
    _SCULK_READ(stream.readUnsignedInt(mSeaBlock));
    return stream.readSignedInt(mSeaFloorDepth);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
