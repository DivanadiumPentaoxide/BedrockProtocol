// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeCappedSurfaceData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeCappedSurfaceData::write(BinaryStream& stream) const {
    stream.writeArray(mFloorBlocks, &BinaryStream::writeSignedInt);
    stream.writeArray(mCeilingBlocks, &BinaryStream::writeSignedInt);
    stream.writeOptional(mSeaBlock, &BinaryStream::writeUnsignedInt);
    stream.writeOptional(mFoundationBlock, &BinaryStream::writeUnsignedInt);
    stream.writeOptional(mBeachBlock, &BinaryStream::writeUnsignedInt);
}

Result<> BiomeCappedSurfaceData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mFloorBlocks, &ReadOnlyBinaryStream::readSignedInt));
    _SCULK_READ(stream.readArray(mCeilingBlocks, &ReadOnlyBinaryStream::readSignedInt));
    _SCULK_READ(stream.readOptional(mSeaBlock, &ReadOnlyBinaryStream::readUnsignedInt));
    _SCULK_READ(stream.readOptional(mFoundationBlock, &ReadOnlyBinaryStream::readUnsignedInt));
    return stream.readOptional(mBeachBlock, &ReadOnlyBinaryStream::readUnsignedInt);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
