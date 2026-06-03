// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "InventoryMismatchTransaction.hpp"
#include "InventoryNormalTransaction.hpp"
#include "ItemReleaseInventoryTransaction.hpp"
#include "ItemUseInventoryTransaction.hpp"
#include "ItemUseOnActorInventoryTransaction.hpp"
#include <variant>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

using InventoryTransactionData = std::variant<
    InventoryNormalTransaction,
    InventoryMismatchTransaction,
    ItemUseInventoryTransaction,
    ItemUseOnActorInventoryTransaction,
    ItemReleaseInventoryTransaction>;

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
