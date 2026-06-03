// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/EmoteListPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds EmoteListPacket::getId() const noexcept { return MinecraftPacketIds::EmoteList; }

std::string_view EmoteListPacket::getName() const noexcept { return "EmoteListPacket"; }

void EmoteListPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeArray(mEmotePieceIds, &UUID::write);
}

Result<> EmoteListPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    return stream.readArray(mEmotePieceIds, &UUID::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string EmoteListPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mActorRuntimeId), SCULK_FORMAT_FIELD(mEmotePieceIds));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
