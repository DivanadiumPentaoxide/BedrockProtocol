// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientboundControlSchemeSetPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientboundControlSchemeSetPacket::getId() const noexcept {
    return MinecraftPacketIds::ClientboundControlSchemeSet;
}

std::string_view ClientboundControlSchemeSetPacket::getName() const noexcept {
    return "ClientboundControlSchemeSetPacket";
}

void ClientboundControlSchemeSetPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mControlScheme, &BinaryStream::writeByte);
}

Result<> ClientboundControlSchemeSetPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readEnum(mControlScheme, &ReadOnlyBinaryStream::readByte);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientboundControlSchemeSetPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mControlScheme));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
