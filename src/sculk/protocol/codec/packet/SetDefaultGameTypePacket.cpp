// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetDefaultGameTypePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetDefaultGameTypePacket::getId() const noexcept { return MinecraftPacketIds::SetDefaultGameType; }

std::string_view SetDefaultGameTypePacket::getName() const noexcept { return "SetDefaultGameTypePacket"; }

void SetDefaultGameTypePacket::write(BinaryStream& stream) const {
    stream.writeEnum(mDefaultGameType, &BinaryStream::writeVarInt);
}

Result<> SetDefaultGameTypePacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readEnum(mDefaultGameType, &ReadOnlyBinaryStream::readVarInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetDefaultGameTypePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mDefaultGameType));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
