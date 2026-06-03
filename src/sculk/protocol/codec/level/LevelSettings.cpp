// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/LevelSettings.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void LevelSettings::write(BinaryStream& stream) const {
    stream.writeUnsignedInt64(mSeed);
    mSpawnSettings.write(stream);
    stream.writeVarInt(mGeneratorType);
    stream.writeEnum(mGameType, &BinaryStream::writeVarInt);
    stream.writeBool(mIsHardCore);
    stream.writeEnum(mDifficulty, &BinaryStream::writeVarInt);
    mSpawnPosition.write(stream);
    stream.writeBool(mAchievementDisabled);
    stream.writeVarInt(mEditorWorldType);
    stream.writeBool(mIsCreatedInEditor);
    stream.writeBool(mIsExportedFromEditor);
    stream.writeVarInt(mDayCycleStopTime);
    stream.writeVarInt(mEduOffer);
    stream.writeBool(mIsEdu);
    stream.writeString(mEduProductId);
    stream.writeFloat(mRainLevel);
    stream.writeFloat(mLightningLevel);
    stream.writeBool(mPlatformLocked);
    stream.writeBool(mMultiplayer);
    stream.writeBool(mLanBroadcast);
    stream.writeVarInt(mXboxLiveBroadcastSetting);
    stream.writeVarInt(mPlatformBroadcastSetting);
    stream.writeBool(mCommandsEnabled);
    stream.writeBool(mTextureRequired);
    stream.writeArray(mGameRules, &GameRuleData::writeLevelSettings);
    mExperiments.write(stream);
    stream.writeBool(mBonusChest);
    stream.writeBool(mStartsWithMap);
    stream.writeEnum(mPlayerPermission, &BinaryStream::writeVarInt);
    stream.writeSignedInt(mTickRange);
    stream.writeBool(mLockBehaviorPack);
    stream.writeBool(mLockResourcePack);
    stream.writeBool(mIsFromLockedTemplate);
    stream.writeBool(mOnlyMsaGamertags);
    stream.writeBool(mIsFromWorldTemplate);
    stream.writeBool(mIsWorldTemplateOptionLocked);
    stream.writeBool(mSpawnV1Villagers);
    stream.writeBool(mPersonaDisabled);
    stream.writeBool(mCustomSkinsDisabled);
    stream.writeBool(mEmoteChatMuted);
    stream.writeString(mBaseGameVersion);
    stream.writeSignedInt(mLimitedWorldWidth);
    stream.writeSignedInt(mLimitedWorldDepth);
    stream.writeBool(mNetherType);
    stream.writeString(mEduResourceButtonName);
    stream.writeString(mEduResourceUri);
    stream.writeBool(mForceExperimentalGameplay);
    stream.writeByte(mChatRestrictionLevel);
    stream.writeBool(mDisablePlayerInteractions);
}

Result<> LevelSettings::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt64(mSeed));
    _SCULK_READ(mSpawnSettings.read(stream));
    _SCULK_READ(stream.readVarInt(mGeneratorType));
    _SCULK_READ(stream.readEnum(mGameType, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readBool(mIsHardCore));
    _SCULK_READ(stream.readEnum(mDifficulty, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(mSpawnPosition.read(stream));
    _SCULK_READ(stream.readBool(mAchievementDisabled));
    _SCULK_READ(stream.readVarInt(mEditorWorldType));
    _SCULK_READ(stream.readBool(mIsCreatedInEditor));
    _SCULK_READ(stream.readBool(mIsExportedFromEditor));
    _SCULK_READ(stream.readVarInt(mDayCycleStopTime));
    _SCULK_READ(stream.readVarInt(mEduOffer));
    _SCULK_READ(stream.readBool(mIsEdu));
    _SCULK_READ(stream.readString(mEduProductId));
    _SCULK_READ(stream.readFloat(mRainLevel));
    _SCULK_READ(stream.readFloat(mLightningLevel));
    _SCULK_READ(stream.readBool(mPlatformLocked));
    _SCULK_READ(stream.readBool(mMultiplayer));
    _SCULK_READ(stream.readBool(mLanBroadcast));
    _SCULK_READ(stream.readVarInt(mXboxLiveBroadcastSetting));
    _SCULK_READ(stream.readVarInt(mPlatformBroadcastSetting));
    _SCULK_READ(stream.readBool(mCommandsEnabled));
    _SCULK_READ(stream.readBool(mTextureRequired));
    _SCULK_READ(stream.readArray(mGameRules, &GameRuleData::readLevelSettings));
    _SCULK_READ(mExperiments.read(stream));
    _SCULK_READ(stream.readBool(mBonusChest));
    _SCULK_READ(stream.readBool(mStartsWithMap));
    _SCULK_READ(stream.readEnum(mPlayerPermission, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readSignedInt(mTickRange));
    _SCULK_READ(stream.readBool(mLockBehaviorPack));
    _SCULK_READ(stream.readBool(mLockResourcePack));
    _SCULK_READ(stream.readBool(mIsFromLockedTemplate));
    _SCULK_READ(stream.readBool(mOnlyMsaGamertags));
    _SCULK_READ(stream.readBool(mIsFromWorldTemplate));
    _SCULK_READ(stream.readBool(mIsWorldTemplateOptionLocked));
    _SCULK_READ(stream.readBool(mSpawnV1Villagers));
    _SCULK_READ(stream.readBool(mPersonaDisabled));
    _SCULK_READ(stream.readBool(mCustomSkinsDisabled));
    _SCULK_READ(stream.readBool(mEmoteChatMuted));
    _SCULK_READ(stream.readString(mBaseGameVersion));
    _SCULK_READ(stream.readSignedInt(mLimitedWorldWidth));
    _SCULK_READ(stream.readSignedInt(mLimitedWorldDepth));
    _SCULK_READ(stream.readBool(mNetherType));
    _SCULK_READ(stream.readString(mEduResourceButtonName));
    _SCULK_READ(stream.readString(mEduResourceUri));
    _SCULK_READ(stream.readBool(mForceExperimentalGameplay));
    _SCULK_READ(stream.readByte(mChatRestrictionLevel));
    return stream.readBool(mDisablePlayerInteractions);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
