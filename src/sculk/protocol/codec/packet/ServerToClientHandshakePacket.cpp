// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ServerToClientHandshakePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ServerToClientHandshakePacket::getId() const noexcept {
    return MinecraftPacketIds::ServerToClientHandshake;
}

std::string_view ServerToClientHandshakePacket::getName() const noexcept { return "ServerToClientHandshakePacket"; }

void ServerToClientHandshakePacket::write(BinaryStream& stream) const { stream.writeString(mHandshakeWebToken); }

Result<> ServerToClientHandshakePacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readString(mHandshakeWebToken);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ServerToClientHandshakePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mHandshakeWebToken));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
