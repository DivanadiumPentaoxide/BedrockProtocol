// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ActorPickRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ActorPickRequestPacket::getId() const noexcept { return MinecraftPacketIds::ActorPickRequest; }

std::string_view ActorPickRequestPacket::getName() const noexcept { return "ActorPickRequestPacket"; }

void ActorPickRequestPacket::write(BinaryStream& stream) const {
    stream.writeSignedInt64(mActorId);
    stream.writeByte(mMaxSlots);
    stream.writeBool(mWithData);
}

Result<> ActorPickRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedInt64(mActorId));
    _SCULK_READ(stream.readByte(mMaxSlots));
    return stream.readBool(mWithData);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ActorPickRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorId),
        SCULK_FORMAT_FIELD(mMaxSlots),
        SCULK_FORMAT_FIELD(mWithData)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
