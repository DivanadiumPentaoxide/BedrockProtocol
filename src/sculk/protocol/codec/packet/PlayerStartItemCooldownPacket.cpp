// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerStartItemCooldownPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerStartItemCooldownPacket::getId() const noexcept {
    return MinecraftPacketIds::PlayerStartItemCooldown;
}

std::string_view PlayerStartItemCooldownPacket::getName() const noexcept { return "PlayerStartItemCooldownPacket"; }

void PlayerStartItemCooldownPacket::write(BinaryStream& stream) const {
    stream.writeString(mItemCategory);
    stream.writeVarInt(mDurationTicks);
}

Result<> PlayerStartItemCooldownPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mItemCategory));
    return stream.readVarInt(mDurationTicks);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerStartItemCooldownPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mItemCategory), SCULK_FORMAT_FIELD(mDurationTicks));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
