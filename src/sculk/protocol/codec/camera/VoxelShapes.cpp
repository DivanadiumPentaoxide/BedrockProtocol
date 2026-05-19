// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/block/VoxelShapes.hpp"

namespace sculk::protocol::inline abi_v975 {

void SerializableVoxelShape::SerializableCells::write(BinaryStream& stream) const {
    stream.writeByte(mXSize);
    stream.writeByte(mYSize);
    stream.writeByte(mZSize);
    stream.writeArray(mStorage, &BinaryStream::writeByte);
}

Result<> SerializableVoxelShape::SerializableCells::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readByte(mXSize));
    _SCULK_READ(stream.readByte(mYSize));
    _SCULK_READ(stream.readByte(mZSize));
    return stream.readArray(mStorage, &ReadOnlyBinaryStream::readByte);
}

void SerializableVoxelShape::write(BinaryStream& stream) const {
    mCells.write(stream);
    stream.writeArray(mXCoordinates, &BinaryStream::writeFloat);
    stream.writeArray(mYCoordinates, &BinaryStream::writeFloat);
    stream.writeArray(mZCoordinates, &BinaryStream::writeFloat);
}

Result<> SerializableVoxelShape::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mCells.read(stream));
    _SCULK_READ(stream.readArray(mXCoordinates, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readArray(mYCoordinates, &ReadOnlyBinaryStream::readFloat));
    return stream.readArray(mZCoordinates, &ReadOnlyBinaryStream::readFloat);
}

} // namespace sculk::protocol::inline abi_v975
