// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/Result.hpp"
#include <string>
#include <vector>

namespace sculk::protocol::inline abi_v975 {

class ClientProperties {
public:
    struct Header {
        std::string alg{};
        std::string x5u{};
    };

    struct Payload {
        struct AnimatedImage {
            int         mAnimationExpression{};
            float       mFrames{};
            std::string mImage{};
            int         mImageHeight{};
            int         mImageWidth{};
            int         mType{};
        };

        struct PersonaPiece {
            bool        mIsDefault{};
            std::string mPackId{};
            std::string mPieceId{};
            std::string mPieceType{};
            std::string mProductId{};
        };

        struct PieceTintColor {
            std::vector<std::string> mColors{};
            std::string              mPieceType{};
        };

        std::vector<AnimatedImage>  mAnimatedImageData{};
        std::string                 mArmSize{};
        std::string                 mCapeData{};
        std::string                 mCapeId{};
        int                         mCapeImageHeight{};
        int                         mCapeImageWidth{};
        bool                        mCapeOnClassicSkin{};
        uint64_t                    mClientRandomId{};
        bool                        mCompatibleWithClientSideChunkGen{};
        int                         mCurrentInputMode{};
        int                         mDefaultInputMode{};
        std::string                 mDeviceId{};
        std::string                 mDeviceModel{};
        int                         mDeviceOS{};
        std::string                 mGameVersion{};
        int                         mGraphicsMode{};
        int                         mGuiScale{};
        bool                        mIsEditorMode{};
        std::string                 mLanguageCode{};
        int                         mMaxViewDistance{};
        int                         mMemoryTier{};
        bool                        mOverrideSkin{};
        std::vector<PersonaPiece>   mPersonaPieces{};
        bool                        mPersonaSkin{};
        std::vector<PieceTintColor> mPieceTintColors{};
        std::string                 mPlatformOfflineId{};
        std::string                 mPlatformOnlineId{};
        int                         mPlatformType{};
        bool                        mPremiumSkin{};
        std::string                 mSelfSignedId{};
        std::string                 mServerAddress{};
        std::string                 mSkinAnimationData{};
        std::string                 mSkinColor{};
        std::string                 mSkinData{};
        std::string                 mSkinGeometryData{};
        std::string                 mSkinGeometryDataEngineVersion{};
        std::string                 mSkinId{};
        int                         mSkinImageHeight{};
        int                         mSkinImageWidth{};
        std::string                 mSkinResourcePatch{};
        std::string                 mThirdPartyName{};
        bool                        mTrustedSkin{};
        int                         mUIProfile{};
    };

public:
    std::string mRawHeader{};
    Header      mHeader{};
    std::string mRawPayload{};
    Payload     mPayload{};
    std::string mSignature{};

public:
    [[nodiscard]] std::string toString() const;

public:
    [[nodiscard]] static Result<ClientProperties> fromString(std::string_view rawClientProperties);
};

} // namespace sculk::protocol::inline abi_v975