// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/HurtArmorPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds HurtArmorPacket::getId() const noexcept { return MinecraftPacketIds::HurtArmor; }

std::string_view HurtArmorPacket::getName() const noexcept { return "HurtArmorPacket"; }

void HurtArmorPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mCause, &BinaryStream::writeVarInt);
    stream.writeVarInt(mDamage);
    stream.writeUnsignedVarInt64(mArmorSlots);
}

Result<> HurtArmorPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mCause, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readVarInt(mDamage));
    return stream.readUnsignedVarInt64(mArmorSlots);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string HurtArmorPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mCause),
        SCULK_FORMAT_FIELD(mDamage),
        SCULK_FORMAT_FIELD(mArmorSlots)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
