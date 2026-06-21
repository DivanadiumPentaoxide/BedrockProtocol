// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/PrimitiveShapes.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void PrimitiveShapes::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mNetworkId);
    stream.writeOptional(mType, [](BinaryStream& stream, PrimitiveShapesType value) {
        stream.writeEnum(value, &BinaryStream::writeByte);
    });
    stream.writeOptional(mLocation, &Vec3::write);
    stream.writeOptional(mScale, &BinaryStream::writeFloat);
    stream.writeOptional(mRotation, &Vec3::write);
    stream.writeOptional(mTotalTimeLeft, &BinaryStream::writeFloat);
    stream.writeOptional(mMaxRenderDistance, &BinaryStream::writeFloat);
    stream.writeOptional(mColor, &BinaryStream::writeSignedInt);
    stream.writeOptional(mDimensionId, &BinaryStream::writeVarInt);
    stream.writeOptional(mAttachedToEntityId, &BinaryStream::writeVarInt64);
    stream.writeVariant(
        mShape,
        Overload{
            [&](const LineDataPayload& line) { line.mLineEndLocation.write(stream); },
            [&](const BoxDataPayload& box) { box.mBoxBound.write(stream); },
            [&](const SphereDataPayload& sphere) { stream.writeByte(sphere.mSegments); },
            [&](const TextDataPayload& text) {
                stream.writeString(text.mText);
                stream.writeBool(text.mUseRotation);
                stream.writeOptional(text.mBackgroundColor, &BinaryStream::writeSignedInt);
                stream.writeBool(text.mDepthTest);
                stream.writeBool(text.mShowBackface);
                stream.writeBool(text.mShowTextBackface);
            },
            [&](const ArrowDataPayload& arrow) {
                stream.writeOptional(arrow.mArrowEndLocation, &Vec3::write);
                stream.writeOptional(arrow.mArrowHeadLength, &BinaryStream::writeFloat);
                stream.writeOptional(arrow.mArrowHeadRadius, &BinaryStream::writeFloat);
                stream.writeOptional(arrow.mArrowSegments, &BinaryStream::writeByte);
            },
            [&](const CylinderDataPayload& cylinder) {
                cylinder.mRadiusX.write(stream);
                cylinder.mRadiusZ.write(stream);
                stream.writeFloat(cylinder.mHeight);
                stream.writeByte(cylinder.mNumSegments);
            },
            [&](const PyramidDataPayload& pyramid) {
                stream.writeFloat(pyramid.mWidth);
                stream.writeOptional(pyramid.mDepth, &BinaryStream::writeFloat);
                stream.writeFloat(pyramid.mHeight);
            },
            [&](const EllipsoidDataPayload& ellipsoid) {
                ellipsoid.mRadii.write(stream);
                stream.writeByte(ellipsoid.mSegmentsPerAxis);
            },
            [&](const ConeDataPayload& cone) {
                cone.mRadii.write(stream);
                stream.writeFloat(cone.mHeight);
                stream.writeByte(cone.mNumSegments);
            },
            [&](const auto&) {}
        }
    );
}

Result<> PrimitiveShapes::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mNetworkId));
    _SCULK_READ(stream.readOptional(mType, [](ReadOnlyBinaryStream& stream, PrimitiveShapesType& value) {
        return stream.readEnum(value, &ReadOnlyBinaryStream::readByte);
    }));
    _SCULK_READ(stream.readOptional(mLocation, &Vec3::read));
    _SCULK_READ(stream.readOptional(mScale, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mRotation, &Vec3::read));
    _SCULK_READ(stream.readOptional(mTotalTimeLeft, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mMaxRenderDistance, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readOptional(mColor, &ReadOnlyBinaryStream::readSignedInt));
    _SCULK_READ(stream.readOptional(mDimensionId, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readOptional(mAttachedToEntityId, &ReadOnlyBinaryStream::readVarInt64));
    return stream.readVariant(
        mShape,
        Overload{
            [&](LineDataPayload& line) { return line.mLineEndLocation.read(stream); },
            [&](BoxDataPayload& box) { return box.mBoxBound.read(stream); },
            [&](SphereDataPayload& sphere) { return stream.readByte(sphere.mSegments); },
            [&](TextDataPayload& text) {
                _SCULK_READ(stream.readString(text.mText));
                _SCULK_READ(stream.readBool(text.mUseRotation));
                _SCULK_READ(stream.readOptional(text.mBackgroundColor, &ReadOnlyBinaryStream::readSignedInt));
                _SCULK_READ(stream.readBool(text.mDepthTest));
                _SCULK_READ(stream.readBool(text.mShowBackface));
                return stream.readBool(text.mShowTextBackface);
            },
            [&](ArrowDataPayload& arrow) {
                _SCULK_READ(stream.readOptional(arrow.mArrowEndLocation, &Vec3::read));
                _SCULK_READ(stream.readOptional(arrow.mArrowHeadLength, &ReadOnlyBinaryStream::readFloat));
                _SCULK_READ(stream.readOptional(arrow.mArrowHeadRadius, &ReadOnlyBinaryStream::readFloat));
                return stream.readOptional(arrow.mArrowSegments, &ReadOnlyBinaryStream::readByte);
            },
            [&](CylinderDataPayload& cylinder) {
                _SCULK_READ(cylinder.mRadiusX.read(stream));
                _SCULK_READ(cylinder.mRadiusZ.read(stream));
                _SCULK_READ(stream.readFloat(cylinder.mHeight));
                return stream.readByte(cylinder.mNumSegments);
            },
            [&](PyramidDataPayload& pyramid) {
                _SCULK_READ(stream.readFloat(pyramid.mWidth));
                _SCULK_READ(stream.readOptional(pyramid.mDepth, &ReadOnlyBinaryStream::readFloat));
                return stream.readFloat(pyramid.mHeight);
            },
            [&](EllipsoidDataPayload& ellipsoid) {
                _SCULK_READ(ellipsoid.mRadii.read(stream));
                return stream.readByte(ellipsoid.mSegmentsPerAxis);
            },
            [&](ConeDataPayload& cone) {
                _SCULK_READ(cone.mRadii.read(stream));
                _SCULK_READ(stream.readFloat(cone.mHeight));
                return stream.readByte(cone.mNumSegments);
            },
            [&](auto&) { return Result<>{}; }
        }
    );
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
