// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/UpdatePlayerGameTypePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds UpdatePlayerGameTypePacket::getId() const noexcept {
    return MinecraftPacketIds::UpdatePlayerGameType;
}

std::string_view UpdatePlayerGameTypePacket::getName() const noexcept { return "UpdatePlayerGameTypePacket"; }

void UpdatePlayerGameTypePacket::write(BinaryStream& stream) const {
    stream.writeEnum(mGameType, &BinaryStream::writeVarInt);
    stream.writeVarInt64(mPlayerUniqueId);
    stream.writeUnsignedVarInt64(mTick);
}

Result<> UpdatePlayerGameTypePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mGameType, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readVarInt64(mPlayerUniqueId));
    return stream.readUnsignedVarInt64(mTick);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string UpdatePlayerGameTypePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mGameType),
        SCULK_FORMAT_FIELD(mPlayerUniqueId),
        SCULK_FORMAT_FIELD(mTick)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
