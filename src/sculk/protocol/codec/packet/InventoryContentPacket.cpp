// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/InventoryContentPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds InventoryContentPacket::getId() const noexcept { return MinecraftPacketIds::InventoryContent; }

std::string_view InventoryContentPacket::getName() const noexcept { return "InventoryContentPacket"; }

void InventoryContentPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(mInventoryId);
    stream.writeArray(mSlots, &NetworkItemStackDescriptor::write);
    mFullContainerName.write(stream);
    mStorageItem.write(stream);
}

Result<> InventoryContentPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt(mInventoryId));
    _SCULK_READ(stream.readArray(mSlots, &NetworkItemStackDescriptor::read));
    _SCULK_READ(mFullContainerName.read(stream));
    return mStorageItem.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string InventoryContentPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mInventoryId),
        SCULK_FORMAT_FIELD(mSlots),
        SCULK_FORMAT_FIELD(mFullContainerName),
        SCULK_FORMAT_FIELD(mStorageItem)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
