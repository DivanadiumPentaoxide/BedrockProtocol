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

enum class ControlScheme : std::uint8_t {
    LockedPlayerRelativeStrafe = 0,
    CameraRelative             = 1,
    CameraRelativeStrafe       = 2,
    PlayerRelative             = 3,
    PlayerRelativeStrafe       = 4,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
