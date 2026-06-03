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
    stream.writeVariantIndex<std::uint32_t>(mShape, &BinaryStream::writeUnsignedVarInt);
    std::visit(
        Overload{
            [&](const PrimitiveLine& line) { line.mLineEndLocation.write(stream); },
            [&](const PrimitiveBox& box) { box.mBoxBound.write(stream); },
            [&](const PrimitiveSegments& segments) { stream.writeByte(segments.mSegments); },
            [&](const PrimitiveText& text) {
                stream.writeString(text.mText);
                stream.writeBool(text.mUseRotation);
                stream.writeOptional(text.mBackgroundColor, &BinaryStream::writeSignedInt);
                stream.writeBool(text.mDepthTest);
                stream.writeBool(text.mShowBackface);
                stream.writeBool(text.mShowTextBackface);
            },
            [&](const PrimitiveArrow& arrow) {
                stream.writeOptional(arrow.mArrowEndLocation, &Vec3::write);
                stream.writeOptional(arrow.mArrowHeadLength, &BinaryStream::writeFloat);
                stream.writeOptional(arrow.mArrowHeadRadius, &BinaryStream::writeFloat);
                stream.writeOptional(arrow.mArrowSegments, &BinaryStream::writeByte);
            },
            [&](const auto&) {}
        },
        mShape
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
    _SCULK_READ(stream.readVariantIndex<std::uint32_t>(mShape, &ReadOnlyBinaryStream::readUnsignedVarInt));
    return std::visit(
        Overload{
            [&](PrimitiveLine& line) { return line.mLineEndLocation.read(stream); },
            [&](PrimitiveBox& box) { return box.mBoxBound.read(stream); },
            [&](PrimitiveSegments& segments) { return stream.readByte(segments.mSegments); },
            [&](PrimitiveText& text) {
                _SCULK_READ(stream.readString(text.mText));
                _SCULK_READ(stream.readBool(text.mUseRotation));
                _SCULK_READ(stream.readOptional(text.mBackgroundColor, &ReadOnlyBinaryStream::readSignedInt));
                _SCULK_READ(stream.readBool(text.mDepthTest));
                _SCULK_READ(stream.readBool(text.mShowBackface));
                return stream.readBool(text.mShowTextBackface);
            },
            [&](PrimitiveArrow& arrow) {
                _SCULK_READ(stream.readOptional(arrow.mArrowEndLocation, &Vec3::read));
                _SCULK_READ(stream.readOptional(arrow.mArrowHeadLength, &ReadOnlyBinaryStream::readFloat));
                _SCULK_READ(stream.readOptional(arrow.mArrowHeadRadius, &ReadOnlyBinaryStream::readFloat));
                return stream.readOptional(arrow.mArrowSegments, &ReadOnlyBinaryStream::readByte);
            },
            [&](auto&) { return Result<>{}; }
        },
        mShape
    );
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
