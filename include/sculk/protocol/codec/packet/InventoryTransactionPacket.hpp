// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/inventory/container/LegacySetItemSlot.hpp"
#include "sculk/protocol/codec/inventory/transaction/InventoryTransactionAction.hpp"
#include "sculk/protocol/codec/inventory/transaction/InventoryTransactionData.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class InventoryTransactionPacket : public IPacket {
public:
    std::int32_t                   mLegacyRequestRawId{};
    std::vector<LegacySetItemSlot> mLegacySetItemSlots{};
    InventoryTransactionActions    mInventoryTransactionActions{};
    InventoryTransactionData       mTransactionData{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(InventoryTransactionPacket)