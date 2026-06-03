// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class CommandOriginType : int {
    Player                   = 0,
    CommandBlock             = 1,
    MinecartCommandBlock     = 2,
    DevConsole               = 3,
    Test                     = 4,
    AutomationPlayer         = 5,
    ClientAutomation         = 6,
    DedicatedServer          = 7,
    Entity                   = 8,
    Virtual                  = 9,
    GameArgument             = 10,
    EntityServer             = 11,
    Precompiled              = 12,
    GameDirectorEntityServer = 13,
    Scripting                = 14,
    ExecuteContext           = 15,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
