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

enum class BlockEventType : std::int32_t {
    EntityFallOn     = 0,
    Place            = 1,
    StateChange      = 2,
    PlayerDestroy    = 3,
    PlayerInteract   = 4,
    PlayerPlacing    = 5,
    QueuedTick       = 6,
    RandomTick       = 7,
    RandomTickLegacy = 8,
    StepOff          = 9,
    StepOn           = 10,
    IsValidSpawn     = 11,
    BlockBreak       = 12,
    RedstoneUpdate   = 13,
    Actor            = 14,
    InternalActor    = 15,
    Count            = 16,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
