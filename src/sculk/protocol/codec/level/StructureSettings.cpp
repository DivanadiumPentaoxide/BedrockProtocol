// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/StructureSettings.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void StructureSettings::write(BinaryStream& stream) const {
    stream.writeString(mPaletteName);
    stream.writeBool(mShouldIgnoreEntities);
    stream.writeBool(mShouldIgnoreBlocks);
    stream.writeBool(mShouldAllowNonTickingPlayerAndTickingAreaChunks);
    mSize.write(stream);
    mOffset.write(stream);
    stream.writeVarInt64(mLastEditPlayer);
    stream.writeEnum(mRotation, &BinaryStream::writeByte);
    stream.writeEnum(mMirror, &BinaryStream::writeByte);
    stream.writeEnum(mAnimationMode, &BinaryStream::writeByte);
    stream.writeFloat(mAnimationSeconds);
    stream.writeFloat(mIntegretyValue);
    stream.writeUnsignedInt(mSeed);
    mRotationPivot.write(stream);
}

Result<> StructureSettings::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mPaletteName));
    _SCULK_READ(stream.readBool(mShouldIgnoreEntities));
    _SCULK_READ(stream.readBool(mShouldIgnoreBlocks));
    _SCULK_READ(stream.readBool(mShouldAllowNonTickingPlayerAndTickingAreaChunks));
    _SCULK_READ(mSize.read(stream));
    _SCULK_READ(mOffset.read(stream));
    _SCULK_READ(stream.readVarInt64(mLastEditPlayer));
    _SCULK_READ(stream.readEnum(mRotation, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readEnum(mMirror, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readEnum(mAnimationMode, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readFloat(mAnimationSeconds));
    _SCULK_READ(stream.readFloat(mIntegretyValue));
    _SCULK_READ(stream.readUnsignedInt(mSeed));
    return mRotationPivot.read(stream);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
