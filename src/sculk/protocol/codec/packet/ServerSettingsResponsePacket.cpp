// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ServerSettingsResponsePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ServerSettingsResponsePacket::getId() const noexcept {
    return MinecraftPacketIds::ServerSettingsResponse;
}

std::string_view ServerSettingsResponsePacket::getName() const noexcept { return "ServerSettingsResponsePacket"; }

void ServerSettingsResponsePacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(mFormId);
    stream.writeString(mFormJson);
}

Result<> ServerSettingsResponsePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt(mFormId));
    return stream.readString(mFormJson);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ServerSettingsResponsePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mFormId), SCULK_FORMAT_FIELD(mFormJson));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
