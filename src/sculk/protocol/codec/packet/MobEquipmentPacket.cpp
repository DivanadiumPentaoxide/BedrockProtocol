// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MobEquipmentPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds MobEquipmentPacket::getId() const noexcept { return MinecraftPacketIds::MobEquipment; }

std::string_view MobEquipmentPacket::getName() const noexcept { return "MobEquipmentPacket"; }

void MobEquipmentPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mRuntimeId);
    mItem.writeCereal(stream);
    stream.writeByte(mSlotByte);
    stream.writeByte(mSelectedSlotByte);
    stream.writeByte(mContainerIdByte);
}

Result<> MobEquipmentPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mRuntimeId));
    _SCULK_READ(mItem.readCereal(stream));
    _SCULK_READ(stream.readByte(mSlotByte));
    _SCULK_READ(stream.readByte(mSelectedSlotByte));
    return stream.readByte(mContainerIdByte);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MobEquipmentPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mRuntimeId),
        SCULK_FORMAT_FIELD(mItem),
        SCULK_FORMAT_FIELD(mSlotByte),
        SCULK_FORMAT_FIELD(mSelectedSlotByte),
        SCULK_FORMAT_FIELD(mContainerIdByte)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
