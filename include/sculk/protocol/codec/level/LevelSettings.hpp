// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "Difficulty.hpp"
#include "sculk/protocol/codec/actor/player/GameType.hpp"
#include "sculk/protocol/codec/actor/player/PlayerPermissionLevel.hpp"
#include "sculk/protocol/codec/level/Experiments.hpp"
#include "sculk/protocol/codec/level/GameRuleData.hpp"
#include "sculk/protocol/codec/level/SpawnSettings.hpp"
#include "sculk/protocol/codec/level/block/BlockPos.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct LevelSettings {
    std::uint64_t             mSeed{};
    SpawnSettings             mSpawnSettings{};
    int                       mGeneratorType{};
    GameType                  mGameType{};
    bool                      mIsHardCore{};
    Difficulty                mDifficulty{};
    BlockPos                  mSpawnPosition{};
    bool                      mAchievementDisabled{};
    int                       mEditorWorldType{};
    bool                      mIsCreatedInEditor{};
    bool                      mIsExportedFromEditor{};
    int                       mDayCycleStopTime{};
    int                       mEduOffer{};
    bool                      mIsEdu{};
    std::string               mEduProductId{};
    float                     mRainLevel{};
    float                     mLightningLevel{};
    bool                      mPlatformLocked{};
    bool                      mMultiplayer{};
    bool                      mLanBroadcast{};
    int                       mXboxLiveBroadcastSetting{};
    int                       mPlatformBroadcastSetting{};
    bool                      mCommandsEnabled{};
    bool                      mTextureRequired{};
    std::vector<GameRuleData> mGameRules{};
    Experiments               mExperiments{};
    bool                      mBonusChest{};
    bool                      mStartsWithMap{};
    PlayerPermissionLevel     mPlayerPermission{};
    std::int32_t              mTickRange{};
    bool                      mLockBehaviorPack{};
    bool                      mLockResourcePack{};
    bool                      mIsFromLockedTemplate{};
    bool                      mOnlyMsaGamertags{};
    bool                      mIsFromWorldTemplate{};
    bool                      mIsWorldTemplateOptionLocked{};
    bool                      mSpawnV1Villagers{};
    bool                      mPersonaDisabled{};
    bool                      mCustomSkinsDisabled{};
    bool                      mEmoteChatMuted{};
    std::string               mBaseGameVersion{};
    std::int32_t              mLimitedWorldWidth{};
    std::int32_t              mLimitedWorldDepth{};
    bool                      mNetherType{};
    std::string               mEduResourceButtonName{};
    std::string               mEduResourceUri{};
    bool                      mForceExperimentalGameplay{};
    std::uint8_t              mChatRestrictionLevel{};
    bool                      mDisablePlayerInteractions{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
