// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class PlayerAuthInputData : int {
    Ascend                          = 0,
    Descend                         = 1,
    NorthJumpDeprecated             = 2,
    JumpDown                        = 3,
    SprintDown                      = 4,
    ChangeHeight                    = 5,
    Jumping                         = 6,
    AutoJumpingInWater              = 7,
    Sneaking                        = 8,
    SneakDown                       = 9,
    Up                              = 10,
    Down                            = 11,
    Left                            = 12,
    Right                           = 13,
    UpLeft                          = 14,
    UpRight                         = 15,
    WantUp                          = 16,
    WantDown                        = 17,
    WantDownSlow                    = 18,
    WantUpSlow                      = 19,
    Sprinting                       = 20,
    AscendBlock                     = 21,
    DescendBlock                    = 22,
    SneakToggleDown                 = 23,
    PersistSneak                    = 24,
    StartSprinting                  = 25,
    StopSprinting                   = 26,
    StartSneaking                   = 27,
    StopSneaking                    = 28,
    StartSwimming                   = 29,
    StopSwimming                    = 30,
    StartJumping                    = 31,
    StartGliding                    = 32,
    StopGliding                     = 33,
    PerformItemInteraction          = 34,
    PerformBlockActions             = 35,
    PerformItemStackRequest         = 36,
    HandledTeleport                 = 37,
    Emoting                         = 38,
    MissedSwing                     = 39,
    StartCrawling                   = 40,
    StopCrawling                    = 41,
    StartFlying                     = 42,
    StopFlying                      = 43,
    ClientAckServerData             = 44,
    IsInClientPredictedVehicle      = 45,
    PaddlingLeft                    = 46,
    PaddlingRight                   = 47,
    BlockBreakingDelayEnabled       = 48,
    HorizontalCollision             = 49,
    VerticalCollision               = 50,
    DownLeft                        = 51,
    DownRight                       = 52,
    StartUsingItem                  = 53,
    IsCameraRelativeMovementEnabled = 54,
    IsRotControlledByMoveDirection  = 55,
    StartSpinAttack                 = 56,
    StopSpinAttack                  = 57,
    IsHotbarOnlyTouch               = 58,
    JumpReleasedRaw                 = 59,
    JumpPressedRaw                  = 60,
    JumpCurrentRaw                  = 61,
    SneakReleasedRaw                = 62,
    SneakPressedRaw                 = 63,
    SneakCurrentRaw                 = 64,
    InputNum                        = 65,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
