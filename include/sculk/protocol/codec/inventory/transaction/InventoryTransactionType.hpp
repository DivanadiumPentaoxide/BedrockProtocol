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

enum class InventoryTransactionType : std::uint32_t {
    NormalTransaction                  = 0,
    InventoryMismatch                  = 1,
    ItemUseInventoryTransaction        = 2,
    ItemUseOnActorInventoryTransaction = 3,
    ItemReleaseInventoryTransaction    = 4,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE