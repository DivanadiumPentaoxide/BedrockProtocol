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
    stream.writeOptional(mLegacySetItemSlots, [&](BinaryStream& stream, const std::vector<LegacySetItemSlot>& slots) {
        stream.writeArray(slots, &LegacySetItemSlot::write);
    });
    stream.writeBool(true); // Unknown flag, should always be true
    stream.writeEnum(mTransaction.type(), &BinaryStream::writeUnsignedVarInt);
    stream.writeBool(true); // Unknown flag, should always be true
    mTransaction.visit([&](const auto& transaction) { transaction.write(stream); });
}

Result<> InventoryTransactionPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mLegacyRequestRawId));
    _SCULK_READ(stream.readOptional(
        mLegacySetItemSlots,
        [&](ReadOnlyBinaryStream& stream, std::vector<LegacySetItemSlot>& slots) {
            return stream.readArray(slots, &LegacySetItemSlot::read);
        }
    ));

    bool unknownFlag1{};
    _SCULK_READ(stream.readBool(unknownFlag1));
    if (!unknownFlag1) {
        return error_utils::makeError("Expected transaction variant");
    }

    InventoryTransactionType transactionType{};
    _SCULK_READ(stream.readEnum(transactionType, &ReadOnlyBinaryStream::readUnsignedVarInt));
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
    _SCULK_READ(mTransaction.set(transactionType, std::source_location::current()));
#else
    _SCULK_READ(mTransaction.set(transactionType));
#endif

    bool unknownFlag2{};
    _SCULK_READ(stream.readBool(unknownFlag2));
    if (!unknownFlag2) {
        return error_utils::makeError("Expected transaction data");
    }

    return mTransaction.visit([&](auto& transaction) { return transaction.read(stream); });
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string InventoryTransactionPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mLegacyRequestRawId),
        SCULK_FORMAT_FIELD(mLegacySetItemSlots),
        SCULK_FORMAT_FIELD(mTransaction)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
