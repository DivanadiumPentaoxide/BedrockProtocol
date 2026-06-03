// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class LegacyTelemetryEventType : int {
    Achievement                     = 0,
    Interaction                     = 1,
    PortalCreated                   = 2,
    PortalUsed                      = 3,
    MobKilled                       = 4,
    CauldronUsed                    = 5,
    PlayerDied                      = 6,
    BossKilled                      = 7,
    AgentCommandObsolete            = 8,
    AgentCreated                    = 9,
    PatternRemovedObsolete          = 10,
    SlashCommand                    = 11,
    DeprecatedFishBucketed          = 12,
    MobBorn                         = 13,
    PetDiedObsolete                 = 14,
    POICauldronUsed                 = 15,
    ComposterUsed                   = 16,
    BellUsed                        = 17,
    ActorDefinition                 = 18,
    RaidUpdate                      = 19,
    PlayerMovementAnomalyObsolete   = 20,
    PlayerMovementCorrectedObsolete = 21,
    HoneyHarvested                  = 22,
    TargetBlockHit                  = 23,
    PiglinBarter                    = 24,
    PlayerWaxedOrUnwaxedCopper      = 25,
    CodeBuilderRuntimeAction        = 26,
    CodeBuilderScoreboard           = 27,
    StriderRiddenInLavaInOverworld  = 28,
    SneakCloseToSculkSensor         = 29,
    CarefulRestoration              = 30,
    ItemUsedEvent                   = 31,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
