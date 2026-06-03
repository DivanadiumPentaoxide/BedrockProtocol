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

enum class AgentActionType : int {
    Attack            = 1,
    Collect           = 2,
    Destroy           = 3,
    DetectRedstone    = 4,
    DetectObstacle    = 5,
    Drop              = 6,
    DropAll           = 7,
    Inspect           = 8,
    InspectData       = 9,
    InspectItemCount  = 10,
    InspectItemDetail = 11,
    InspectItemSpace  = 12,
    Interact          = 13,
    Move              = 14,
    PlaceBlock        = 15,
    Till              = 16,
    TransferItemTo    = 17,
    Turn              = 18,
};

}