// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerArmorDamagePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerArmorDamagePacket::getId() const noexcept { return MinecraftPacketIds::PlayerArmorDamage; }

std::string_view PlayerArmorDamagePacket::getName() const noexcept { return "PlayerArmorDamagePacket"; }

void PlayerArmorDamagePacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(static_cast<uint32_t>(mSlotsBitset.count()));
    for (std::size_t index = 0; index < ArmorSlotCount; index++) {
        if (mSlotsBitset.test(index)) {
            stream.writeByte(static_cast<uint8_t>(index << 1));
            stream.writeSignedShort(mDamage[index]);
        }
    }
}

Result<> PlayerArmorDamagePacket::read(ReadOnlyBinaryStream& stream) {
    std::uint32_t count{};
    _SCULK_READ(stream.readUnsignedVarInt(count));
    mSlotsBitset.reset();
    mDamage.fill(0);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint8_t encodedIndex{};
        std::int16_t damage{};
        _SCULK_READ(stream.readByte(encodedIndex));
        _SCULK_READ(stream.readSignedShort(damage));
        const std::size_t index = encodedIndex >> 1;
        if (index < ArmorSlotCount) {
            mSlotsBitset.set(index, true);
            mDamage[index] = damage;
        }
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerArmorDamagePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mSlotsBitset), SCULK_FORMAT_FIELD(mDamage));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
