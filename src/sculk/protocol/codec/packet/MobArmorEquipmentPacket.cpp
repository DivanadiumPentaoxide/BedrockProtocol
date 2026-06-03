// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MobArmorEquipmentPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds MobArmorEquipmentPacket::getId() const noexcept { return MinecraftPacketIds::MobArmorEquipment; }

std::string_view MobArmorEquipmentPacket::getName() const noexcept { return "MobArmorEquipmentPacket"; }

void MobArmorEquipmentPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mRuntimeId);
    mHead.write(stream);
    mTorso.write(stream);
    mLegs.write(stream);
    mFeet.write(stream);
    mBody.write(stream);
}

Result<> MobArmorEquipmentPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mRuntimeId));
    _SCULK_READ(mHead.read(stream));
    _SCULK_READ(mTorso.read(stream));
    _SCULK_READ(mLegs.read(stream));
    _SCULK_READ(mFeet.read(stream));
    return mBody.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MobArmorEquipmentPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mRuntimeId),
        SCULK_FORMAT_FIELD(mHead),
        SCULK_FORMAT_FIELD(mTorso),
        SCULK_FORMAT_FIELD(mLegs),
        SCULK_FORMAT_FIELD(mFeet),
        SCULK_FORMAT_FIELD(mBody)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
