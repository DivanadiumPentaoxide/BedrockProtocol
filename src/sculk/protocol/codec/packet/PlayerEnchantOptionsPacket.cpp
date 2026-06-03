// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerEnchantOptionsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerEnchantOptionsPacket::getId() const noexcept {
    return MinecraftPacketIds::PlayerEnchantOptions;
}

std::string_view PlayerEnchantOptionsPacket::getName() const noexcept { return "PlayerEnchantOptionsPacket"; }

void PlayerEnchantOptionsPacket::write(BinaryStream& stream) const {
    stream.writeArray(mOptions, &EnchantOption::write);
}

Result<> PlayerEnchantOptionsPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mOptions, &EnchantOption::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerEnchantOptionsPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mOptions)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
