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

enum class ContainerID : std::uint8_t {
    Inventory      = 0,
    First          = 1,
    Last           = 100,
    Offhand        = 119,
    Armor          = 120,
    SelectionSlots = 122,
    PlayerOnlyUi   = 124,
    Registry       = 125,
    None           = 255,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
