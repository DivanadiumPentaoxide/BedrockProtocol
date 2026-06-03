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

enum class ContainerType : std::uint8_t {
    Container          = 0,
    Workbench          = 1,
    Furnace            = 2,
    Enchantment        = 3,
    BrewingStand       = 4,
    Anvil              = 5,
    Dispenser          = 6,
    Dropper            = 7,
    Hopper             = 8,
    Cauldron           = 9,
    MinecartChest      = 10,
    MinecartHopper     = 11,
    Horse              = 12,
    Beacon             = 13,
    StructureEditor    = 14,
    Trade              = 15,
    CommandBlock       = 16,
    Jukebox            = 17,
    Armor              = 18,
    Hand               = 19,
    CompoundCreator    = 20,
    ElementConstructor = 21,
    MaterialReducer    = 22,
    LabTable           = 23,
    Loom               = 24,
    Lectern            = 25,
    Grindstone         = 26,
    BlastFurnace       = 27,
    Smoker             = 28,
    Stonecutter        = 29,
    Cartography        = 30,
    Hud                = 31,
    JigsawEditor       = 32,
    SmithingTable      = 33,
    ChestBoat          = 34,
    DecoratedPot       = 35,
    Crafter            = 36,
    None               = 247,
    Inventory          = 255,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
