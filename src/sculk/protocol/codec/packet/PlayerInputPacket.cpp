// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerInputPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerInputPacket::getId() const noexcept { return MinecraftPacketIds::PlayerInput_Deprecated; }

std::string_view PlayerInputPacket::getName() const noexcept { return "PlayerInputPacket"; }

void PlayerInputPacket::write(BinaryStream& stream) const {
    mMove.write(stream);
    stream.writeBool(mJumping);
    stream.writeBool(mSneaking);
}

Result<> PlayerInputPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mMove.read(stream));
    _SCULK_READ(stream.readBool(mJumping));
    return stream.readBool(mSneaking);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerInputPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mMove)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
