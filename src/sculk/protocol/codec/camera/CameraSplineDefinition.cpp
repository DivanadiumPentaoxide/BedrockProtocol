// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/camera/CameraSplineDefinition.hpp"
#include "../utility/EnumName.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void CameraSplineProgressKeyFrame::write(BinaryStream& stream) const {
    stream.writeFloat(mProgress);
    stream.writeFloat(mTime);
    utils::writeEnumName(stream, mEasing);
}

Result<> CameraSplineProgressKeyFrame::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readFloat(mProgress));
    _SCULK_READ(stream.readFloat(mTime));
    return utils::readEnumName(stream, mEasing);
}

void CameraSplineRotationKeyFrame::write(BinaryStream& stream) const {
    mRotation.write(stream);
    stream.writeFloat(mTime);
    utils::writeEnumName(stream, mEasing);
}

Result<> CameraSplineRotationKeyFrame::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mRotation.read(stream));
    _SCULK_READ(stream.readFloat(mTime));
    return utils::readEnumName(stream, mEasing);
}

void CameraSplineDefinition::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeFloat(mTotalTime);
    stream.writeString(mSplineType);
    stream.writeArray(mControlPoints, &Vec3::write);
    stream.writeArray(mProgressKeyFrames, &CameraSplineProgressKeyFrame::write);
    stream.writeArray(mRotationKeyFrames, &CameraSplineRotationKeyFrame::write);
}

Result<> CameraSplineDefinition::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readFloat(mTotalTime));
    _SCULK_READ(stream.readString(mSplineType));
    _SCULK_READ(stream.readArray(mControlPoints, &Vec3::read));
    _SCULK_READ(stream.readArray(mProgressKeyFrames, &CameraSplineProgressKeyFrame::read));
    return stream.readArray(mRotationKeyFrames, &CameraSplineRotationKeyFrame::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
