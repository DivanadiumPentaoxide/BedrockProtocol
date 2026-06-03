// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerToggleCrafterSlotRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerToggleCrafterSlotRequestPacket::getId() const noexcept {
    return MinecraftPacketIds::PlayerToggleCrafterSlotRequest;
}

std::string_view PlayerToggleCrafterSlotRequestPacket::getName() const noexcept {
    return "PlayerToggleCrafterSlotRequestPacket";
}

void PlayerToggleCrafterSlotRequestPacket::write(BinaryStream& stream) const {
    stream.writeSignedInt(mPosX);
    stream.writeSignedInt(mPosY);
    stream.writeSignedInt(mPosZ);
    stream.writeByte(mSlotIndex);
    stream.writeBool(mIsDisabled);
}

Result<> PlayerToggleCrafterSlotRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedInt(mPosX));
    _SCULK_READ(stream.readSignedInt(mPosY));
    _SCULK_READ(stream.readSignedInt(mPosZ));
    _SCULK_READ(stream.readByte(mSlotIndex));
    return stream.readBool(mIsDisabled);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerToggleCrafterSlotRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPosX),
        SCULK_FORMAT_FIELD(mPosY),
        SCULK_FORMAT_FIELD(mPosZ),
        SCULK_FORMAT_FIELD(mSlotIndex),
        SCULK_FORMAT_FIELD(mIsDisabled)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
