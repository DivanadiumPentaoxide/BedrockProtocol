// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/LevelEventPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds LevelEventPacket::getId() const noexcept { return MinecraftPacketIds::LevelEvent; }

std::string_view LevelEventPacket::getName() const noexcept { return "LevelEventPacket"; }

void LevelEventPacket::write(BinaryStream& stream) const {
    stream.writeVarInt(mEventType);
    mPosition.write(stream);
    stream.writeVarInt(mEventData);
}

Result<> LevelEventPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mEventType));
    _SCULK_READ(mPosition.read(stream));
    return stream.readVarInt(mEventData);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LevelEventPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mEventType),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mEventData)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
