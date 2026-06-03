// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/camera/CameraPreset.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void CameraPreset::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeString(mInhertFrom);
    stream.writeOptional(mPosX, &BinaryStream::writeFloat);
    stream.writeOptional(mPosY, &BinaryStream::writeFloat);
    stream.writeOptional(mPosZ, &BinaryStream::writeFloat);
    stream.writeOptional(mRotX, &BinaryStream::writeFloat);
    stream.writeOptional(mRotY, &BinaryStream::writeFloat);
    stream.writeOptional(mRotationSpeed, &BinaryStream::writeFloat);
    stream.writeOptional(mSnapToTarget, &BinaryStream::writeBool);
    stream.writeOptional(mHorizontalRotationLimit, &Vec2::write);
    stream.writeOptional(mVerticalRotationLimit, &Vec2::write);
    stream.writeOptional(mContinueTargeting, &BinaryStream::writeBool);
    stream.writeOptional(mBlockListingRadius, &BinaryStream::writeFloat);
    stream.writeOptional(mViewOffset, &Vec2::write);
    stream.writeOptional(mEntityOffset, &Vec3::write);
    stream.writeOptional(mRadius, &BinaryStream::writeFloat);
    stream.writeOptional(mYawLimitMin, &BinaryStream::writeFloat);
    stream.writeOptional(mYawLimitMax, &BinaryStream::writeFloat);
    stream.writeOptional(mListener, [](BinaryStream& stream, AudioListener value) {
        stream.writeEnum(value, &BinaryStream::writeByte);
    });
    stream.writeOptional(mPlayerEffects, &BinaryStream::writeBool);
    stream.writeOptional(mAimAssist, &CameraPresetAimAssistDefinition::write);
    stream.writeOptional(mControlScheme, [](BinaryStream& stream, ControlScheme value) {
        stream.writeEnum(value, &BinaryStream::writeByte);
    });
}

Result<> CameraPreset::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readString(mInhertFrom));
    _SCULK_READ(stream.readOptional(mPosX, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mPosY, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mPosZ, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mRotX, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mRotY, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mRotationSpeed, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mSnapToTarget, &ReadOnlyBinaryStream::readBool));
    _SCULK_READ(stream.readOptional(mHorizontalRotationLimit, &Vec2::read));
    _SCULK_READ(stream.readOptional(mVerticalRotationLimit, &Vec2::read));
    _SCULK_READ(stream.readOptional(mContinueTargeting, &ReadOnlyBinaryStream::readBool));
    _SCULK_READ(stream.readOptional(mBlockListingRadius, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mViewOffset, &Vec2::read));
    _SCULK_READ(stream.readOptional(mEntityOffset, &Vec3::read));
    _SCULK_READ(stream.readOptional(mRadius, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mYawLimitMin, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mYawLimitMax, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mListener, [](ReadOnlyBinaryStream& stream, AudioListener& value) {
        return stream.readEnum(value, &ReadOnlyBinaryStream::readByte);
    }));
    _SCULK_READ(stream.readOptional(mPlayerEffects, &ReadOnlyBinaryStream::readBool));
    _SCULK_READ(stream.readOptional(mAimAssist, &CameraPresetAimAssistDefinition::read));
    return stream.readOptional(mControlScheme, [](ReadOnlyBinaryStream& stream, ControlScheme& value) {
        return stream.readEnum(value, &ReadOnlyBinaryStream::readByte);
    });
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
