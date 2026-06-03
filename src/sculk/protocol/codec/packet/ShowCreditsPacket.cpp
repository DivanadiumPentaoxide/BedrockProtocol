// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ShowCreditsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ShowCreditsPacket::getId() const noexcept { return MinecraftPacketIds::ShowCredits; }

std::string_view ShowCreditsPacket::getName() const noexcept { return "ShowCreditsPacket"; }

void ShowCreditsPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mPlayerRuntimeId);
    stream.writeEnum(mCreditsState, &BinaryStream::writeVarInt);
}

Result<> ShowCreditsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mPlayerRuntimeId));
    return stream.readEnum(mCreditsState, &ReadOnlyBinaryStream::readVarInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ShowCreditsPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mPlayerRuntimeId), SCULK_FORMAT_FIELD(mCreditsState));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
