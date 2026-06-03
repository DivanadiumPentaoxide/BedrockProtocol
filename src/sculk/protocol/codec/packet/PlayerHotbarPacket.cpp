// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerHotbarPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerHotbarPacket::getId() const noexcept { return MinecraftPacketIds::PlayerHotbar; }

std::string_view PlayerHotbarPacket::getName() const noexcept { return "PlayerHotbarPacket"; }

void PlayerHotbarPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(mSelectedSlot);
    stream.writeEnum(mContainerId, &BinaryStream::writeByte);
    stream.writeBool(mShouldSelectSlot);
}

Result<> PlayerHotbarPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt(mSelectedSlot));
    _SCULK_READ(stream.readEnum(mContainerId, &ReadOnlyBinaryStream::readByte));
    return stream.readBool(mShouldSelectSlot);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerHotbarPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mSelectedSlot),
        SCULK_FORMAT_FIELD(mContainerId),
        SCULK_FORMAT_FIELD(mShouldSelectSlot)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
