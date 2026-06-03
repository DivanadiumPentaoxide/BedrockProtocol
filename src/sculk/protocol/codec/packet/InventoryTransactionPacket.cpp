// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/InventoryTransactionPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds InventoryTransactionPacket::getId() const noexcept {
    return MinecraftPacketIds::InventoryTransaction;
}

std::string_view InventoryTransactionPacket::getName() const noexcept { return "InventoryTransactionPacket"; }

void InventoryTransactionPacket::write(BinaryStream& stream) const {
    stream.writeVarInt(mLegacyRequestRawId);
    if (mLegacyRequestRawId < -1 && (mLegacyRequestRawId & 1) == 0) {
        stream.writeArray(mLegacySetItemSlots, &LegacySetItemSlot::write);
    }
    stream.writeVariantIndex<std::uint32_t>(mTransactionData, &BinaryStream::writeUnsignedVarInt);
    mInventoryTransactionActions.write(stream);
    std::visit([&](const auto& transaction) { transaction.write(stream); }, mTransactionData);
}

Result<> InventoryTransactionPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mLegacyRequestRawId));
    if (mLegacyRequestRawId < -1 && (mLegacyRequestRawId & 1) == 0) {
        _SCULK_READ(stream.readArray(mLegacySetItemSlots, &LegacySetItemSlot::read));
    }
    _SCULK_READ(stream.readVariantIndex<std::uint32_t>(mTransactionData, &ReadOnlyBinaryStream::readUnsignedVarInt));
    _SCULK_READ(mInventoryTransactionActions.read(stream));
    return std::visit([&](auto& transaction) { return transaction.read(stream); }, mTransactionData);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string InventoryTransactionPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mLegacyRequestRawId),
        SCULK_FORMAT_FIELD(mLegacySetItemSlots),
        SCULK_FORMAT_FIELD(mInventoryTransactionActions),
        SCULK_FORMAT_FIELD(mTransactionData)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
