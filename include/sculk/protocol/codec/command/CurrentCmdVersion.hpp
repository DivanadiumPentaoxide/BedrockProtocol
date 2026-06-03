// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class CurrentCmdVersion : int {
    Invalid                                      = -1,
    Initial                                      = 1,
    TpRotationClamping                           = 2,
    NewBedrockCmdSystem                          = 3,
    ExecuteUsesVec3                              = 4,
    CloneFixes                                   = 5,
    UpdateAquatic                                = 6,
    EntitySelectorUsesVec3                       = 7,
    ContainersDontDropItemsAnymore               = 8,
    FiltersObeyDimensions                        = 9,
    ExecuteAndBlockCommandAndSelfSelectorFixes   = 10,
    InstantEffectsUseTicks                       = 11,
    DontRegisterBrokenFunctionCommands           = 12,
    ClearSpawnPointCommand                       = 13,
    CloneAndTeleportRotationFixes                = 14,
    TeleportDimensionFixes                       = 15,
    CloneUpdateBlockAndTimeFixes                 = 16,
    CloneIntersectFix                            = 17,
    FunctionExecuteOrderAndChestSlotFix          = 18,
    NonTickingAreasNoLongerConsideredLoaded      = 19,
    SpreadplayersHazardAndResolvePlayerByNameFix = 20,
    NewExecuteCommandSyntaxExperimentAndChestLootTableFixAndTeleportFacingVerticalUnclampedAndLocateBiomeAndFeatureMerged =
        21,
    WaterloggingAddedToStructureCommand            = 22,
    SelectorDistanceFilteredAndRelativeRotationFix = 23,
    NewSummonCommandAddedRotationOptionsAndBubbleColumnCloneFixAndExecuteInDimensionTeleportFixAndNewExecuteRotationFix =
        24,
    NewExecuteCommandReleaseEnchantCommandLevelFixAndHasItemDataFixAndCommandDeferral = 25,
    ExecuteIfScoreFixes                                                               = 26,
    ReplaceItemAndLootReplaceBlockCommandsDoNotPlaceItemsIntoCauldronsFix             = 27,
    ChangesToCommandOriginRotation                                                    = 28,
    RemoveAuxValueParameterFromBlockCommands                                          = 29,
    VolumeSelectorFixes                                                               = 30,
    EnableSummonRotation                                                              = 31,
    SummonCommandDefaultRotation                                                      = 32,
    PositionalDimensionFiltering                                                      = 33,
    CommandSelectorHasItemFilterNoLongerCallsSameItemFunction                         = 34,
    AgentSweepingBlockTest                                                            = 35,
    BlockStateEquals                                                                  = 36,
    CommandPositionFix                                                                = 37,
    CommandSelectorHasItemFilterUsesDataAsDamageForSelectingDamageableItems           = 38,
    ExecuteDetectConditionSubcommandNotAllowNonLoadedBlocks                           = 39,
    RemoveSuicideKeyword                                                              = 40,
    CloneContainerBlockEntityRemovalFix                                               = 41,
    StopSoundMusicFix                                                                 = 42,
    SpreadPlayersStuckInGroundFixAndMaxHeightParameter                                = 43,
    LocateStructureOutput                                                             = 44,
    PostBlockFlattening                                                               = 45,
    TestForBlockCommandDoesNotIgnoreBlockState                                        = 46,
    Count                                                                             = 47,
    Latest                                                                            = 48,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
