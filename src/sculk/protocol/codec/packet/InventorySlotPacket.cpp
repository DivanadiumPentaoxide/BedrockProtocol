// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/InventorySlotPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds InventorySlotPacket::getId() const noexcept { return MinecraftPacketIds::InventorySlot; }

std::string_view InventorySlotPacket::getName() const noexcept { return "InventorySlotPacket"; }

void InventorySlotPacket::write(BinaryStream& stream) const {
    stream.writeByte(mInventoryId);
    stream.writeUnsignedVarInt(mSlot);
    stream.writeOptional(mFullContainerName, &FullContainerName::write);
    stream.writeOptional(mStorageItem, &NetworkItemStackDescriptor::writeCereal);
    mItem.writeCereal(stream);
}

Result<> InventorySlotPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readByte(mInventoryId));
    _SCULK_READ(stream.readUnsignedVarInt(mSlot));
    _SCULK_READ(stream.readOptional(mFullContainerName, &FullContainerName::read));
    _SCULK_READ(stream.readOptional(mStorageItem, &NetworkItemStackDescriptor::readCereal));
    return mItem.readCereal(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string InventorySlotPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mInventoryId),
        SCULK_FORMAT_FIELD(mSlot),
        SCULK_FORMAT_FIELD(mFullContainerName),
        SCULK_FORMAT_FIELD(mStorageItem),
        SCULK_FORMAT_FIELD(mItem)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
