// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "InventoryTransactionSource.hpp"
#include "sculk/protocol/codec/inventory/item/NetworkItemStackDescriptor.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct InventoryTransactionAction {
    InventoryTransactionSource mSource{};
    std::uint32_t              mSlot{};
    NetworkItemStackDescriptor mFromItem{};
    NetworkItemStackDescriptor mToItem{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct InventoryTransactionActions {
    std::vector<InventoryTransactionAction> mActions{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};


} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE