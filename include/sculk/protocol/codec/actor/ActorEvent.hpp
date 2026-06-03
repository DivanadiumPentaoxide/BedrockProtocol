// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <cstdint>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class ActorEvent : std::uint8_t {
    None                             = 0,
    Jump                             = 1,
    Hurt                             = 2,
    Death                            = 3,
    StartAttacking                   = 4,
    StopAttacking                    = 5,
    TamingFailed                     = 6,
    TamingSucceeded                  = 7,
    ShakeWetness                     = 8,
    EatGrass                         = 10,
    FishhookBubble                   = 11,
    FishhookFishpos                  = 12,
    FishhookHooktime                 = 13,
    FishhookTease                    = 14,
    SquidFleeing                     = 15,
    ZombieConverting                 = 16,
    PlayAmbient                      = 17,
    SpawnAlive                       = 18,
    StartOfferFlower                 = 19,
    StopOfferFlower                  = 20,
    LoveHearts                       = 21,
    VillagerAngry                    = 22,
    VillagerHappy                    = 23,
    WitchHatMagic                    = 24,
    FireworksExplode                 = 25,
    InLoveHearts                     = 26,
    SilverfishMergeAnim              = 27,
    GuardianAttackSound              = 28,
    DrinkPotion                      = 29,
    ThrowPotion                      = 30,
    PrimeTntcart                     = 31,
    PrimeCreeper                     = 32,
    AirSupply                        = 33,
    DeprecatedAddPlayerLevels        = 34,
    GuardianMiningFatigue            = 35,
    AgentSwingArm                    = 36,
    DragonStartDeathAnim             = 37,
    GroundDust                       = 38,
    Shake                            = 39,
    Feed                             = 57,
    BabyAge                          = 60,
    InstantDeath                     = 61,
    NotifyTrade                      = 62,
    LeashDestroyed                   = 63,
    CaravanUpdated                   = 64,
    TalismanActivate                 = 65,
    DeprecatedUpdateStructureFeature = 66,
    PlayerSpawnedMob                 = 67,
    Puke                             = 68,
    UpdateStackSize                  = 69,
    StartSwimming                    = 70,
    BalloonPop                       = 71,
    TreasureHunt                     = 72,
    SummonAgent                      = 73,
    FinishedChargingItem             = 74,
    ActorGrowUp                      = 76,
    VibrationDetected                = 77,
    DrinkMilk                        = 78,
    ShakeWetnessStop                 = 79,
    KineticDamageDealt               = 80,
    HurtWithoutReceivingDamage       = 81,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
