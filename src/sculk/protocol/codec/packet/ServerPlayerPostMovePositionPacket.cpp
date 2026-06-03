// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ServerPlayerPostMovePositionPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ServerPlayerPostMovePositionPacket::getId() const noexcept {
    return MinecraftPacketIds::ServerPlayerPostMovePosition;
}

std::string_view ServerPlayerPostMovePositionPacket::getName() const noexcept {
    return "ServerPlayerPostMovePositionPacket";
}

void ServerPlayerPostMovePositionPacket::write(BinaryStream& stream) const { mPos.write(stream); }

Result<> ServerPlayerPostMovePositionPacket::read(ReadOnlyBinaryStream& stream) { return mPos.read(stream); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ServerPlayerPostMovePositionPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mPos));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
