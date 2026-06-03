// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetHudPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetHudPacket::getId() const noexcept { return MinecraftPacketIds::SetHud; }

std::string_view SetHudPacket::getName() const noexcept { return "SetHudPacket"; }

void SetHudPacket::write(BinaryStream& stream) const {
    stream.writeArray(mHudElements, [](BinaryStream& stream, const HudElement& element) {
        stream.writeEnum(element, &BinaryStream::writeVarInt);
    });
    stream.writeEnum(mHudVisibility, &BinaryStream::writeVarInt);
}

Result<> SetHudPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mHudElements, [](ReadOnlyBinaryStream& stream, HudElement& element) {
        return stream.readEnum(element, &ReadOnlyBinaryStream::readVarInt);
    }));
    return stream.readEnum(mHudVisibility, &ReadOnlyBinaryStream::readVarInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetHudPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mHudElements), SCULK_FORMAT_FIELD(mHudVisibility));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
