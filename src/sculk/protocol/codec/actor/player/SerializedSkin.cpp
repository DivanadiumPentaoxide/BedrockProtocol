// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/player/SerializedSkin.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void SerializedSkin::Animation::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(mWidth);
    stream.writeUnsignedInt(mHeight);
    stream.writeString(mSkinImageBytes);
    stream.writeEnum(mAnimationType, &BinaryStream::writeUnsignedInt);
    stream.writeFloat(mFrameCount);
    stream.writeEnum(mAnimationExpression, &BinaryStream::writeUnsignedInt);
}

Result<> SerializedSkin::Animation::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt(mWidth));
    _SCULK_READ(stream.readUnsignedInt(mHeight));
    _SCULK_READ(stream.readString(mSkinImageBytes));
    _SCULK_READ(stream.readEnum(mAnimationType, &ReadOnlyBinaryStream::readUnsignedInt));
    _SCULK_READ(stream.readFloat(mFrameCount));
    return stream.readEnum(mAnimationExpression, &ReadOnlyBinaryStream::readUnsignedInt);
}

void SerializedSkin::PersonaPiece::write(BinaryStream& stream) const {
    stream.writeString(mPieceId);
    stream.writeString(mPieceType);
    stream.writeString(mPackId);
    stream.writeBool(mIsDefaultPiece);
    stream.writeString(mProductId);
}

Result<> SerializedSkin::PersonaPiece::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mPieceId));
    _SCULK_READ(stream.readString(mPieceType));
    _SCULK_READ(stream.readString(mPackId));
    _SCULK_READ(stream.readBool(mIsDefaultPiece));
    return stream.readString(mProductId);
}

void SerializedSkin::PieceTintColors::write(BinaryStream& stream) const {
    stream.writeString(mPieceType);
    stream.writeUnsignedInt(static_cast<std::uint32_t>(mPieceTintColors.size()));
    for (const std::string& tintColor : mPieceTintColors) {
        stream.writeString(tintColor);
    }
}

Result<> SerializedSkin::PieceTintColors::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mPieceType));
    std::uint32_t tintColorCount{};
    _SCULK_READ(stream.readUnsignedInt(tintColorCount));
    mPieceTintColors.resize(tintColorCount);
    for (std::string& tintColor : mPieceTintColors) {
        _SCULK_READ(stream.readString(tintColor));
    }
    return {};
}

void SerializedSkin::write(BinaryStream& stream) const {
    stream.writeString(mId);
    stream.writeString(mPlayFabId);
    stream.writeString(mResourcePatch);
    stream.writeUnsignedInt(mSkinImageWidth);
    stream.writeUnsignedInt(mSkinImageHeight);
    stream.writeString(mSkinImageBytes);
    stream.writeUnsignedInt(static_cast<std::uint32_t>(mAnimations.size()));
    for (const Animation& animation : mAnimations) {
        animation.write(stream);
    }
    stream.writeUnsignedInt(mCapeImageWidth);
    stream.writeUnsignedInt(mCapeImageHeight);
    stream.writeString(mCapeImageBytes);
    stream.writeString(mGeometryData);
    stream.writeString(mGeometryDataMinEngineVersion);
    stream.writeString(mAnimationData);
    stream.writeString(mCapeId);
    stream.writeString(mFullId);
    stream.writeString(mArmSize);
    stream.writeString(mSkinColor);
    stream.writeUnsignedInt(static_cast<std::uint32_t>(mPersonaPieces.size()));
    for (const PersonaPiece& piece : mPersonaPieces) {
        piece.write(stream);
    }
    stream.writeUnsignedInt(static_cast<std::uint32_t>(mPieceTintColors.size()));
    for (const PieceTintColors& tintColors : mPieceTintColors) {
        tintColors.write(stream);
    }
    stream.writeBool(mIsPremiumSkin);
    stream.writeBool(mIsPersonaSkin);
    stream.writeBool(mIsPersonaCapeOnClassicSkin);
    stream.writeBool(mIsPrimaryUser);
    stream.writeBool(mOverridesPlayerAppearance);
}

Result<> SerializedSkin::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mId));
    _SCULK_READ(stream.readString(mPlayFabId));
    _SCULK_READ(stream.readString(mResourcePatch));
    _SCULK_READ(stream.readUnsignedInt(mSkinImageWidth));
    _SCULK_READ(stream.readUnsignedInt(mSkinImageHeight));
    _SCULK_READ(stream.readString(mSkinImageBytes));

    std::uint32_t animationCount{};
    _SCULK_READ(stream.readUnsignedInt(animationCount));
    mAnimations.resize(animationCount);
    for (Animation& animation : mAnimations) {
        _SCULK_READ(animation.read(stream));
    }

    _SCULK_READ(stream.readUnsignedInt(mCapeImageWidth));
    _SCULK_READ(stream.readUnsignedInt(mCapeImageHeight));
    _SCULK_READ(stream.readString(mCapeImageBytes));
    _SCULK_READ(stream.readString(mGeometryData));
    _SCULK_READ(stream.readString(mGeometryDataMinEngineVersion));
    _SCULK_READ(stream.readString(mAnimationData));
    _SCULK_READ(stream.readString(mCapeId));
    _SCULK_READ(stream.readString(mFullId));
    _SCULK_READ(stream.readString(mArmSize));
    _SCULK_READ(stream.readString(mSkinColor));

    std::uint32_t personaPieceCount{};
    _SCULK_READ(stream.readUnsignedInt(personaPieceCount));
    mPersonaPieces.resize(personaPieceCount);
    for (PersonaPiece& piece : mPersonaPieces) {
        _SCULK_READ(piece.read(stream));
    }

    std::uint32_t pieceTintColorCount{};
    _SCULK_READ(stream.readUnsignedInt(pieceTintColorCount));
    mPieceTintColors.resize(pieceTintColorCount);
    for (PieceTintColors& tintColors : mPieceTintColors) {
        _SCULK_READ(tintColors.read(stream));
    }

    _SCULK_READ(stream.readBool(mIsPremiumSkin));
    _SCULK_READ(stream.readBool(mIsPersonaSkin));
    _SCULK_READ(stream.readBool(mIsPersonaCapeOnClassicSkin));
    _SCULK_READ(stream.readBool(mIsPrimaryUser));
    return stream.readBool(mOverridesPlayerAppearance);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
