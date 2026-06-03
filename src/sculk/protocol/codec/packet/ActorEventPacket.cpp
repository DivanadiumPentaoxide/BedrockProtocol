// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ActorEventPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ActorEventPacket::getId() const noexcept { return MinecraftPacketIds::ActorEvent; }

std::string_view ActorEventPacket::getName() const noexcept { return "ActorEventPacket"; }

void ActorEventPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mRuntimeId);
    stream.writeEnum(mEventId, &BinaryStream::writeByte);
    stream.writeVarInt(mData);
    stream.writeOptional(mFireAtPosition, &Vec3::write);
}

Result<> ActorEventPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mRuntimeId));
    _SCULK_READ(stream.readEnum(mEventId, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readVarInt(mData));
    return stream.readOptional(mFireAtPosition, &Vec3::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ActorEventPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mRuntimeId),
        SCULK_FORMAT_FIELD(mEventId),
        SCULK_FORMAT_FIELD(mData),
        SCULK_FORMAT_FIELD(mFireAtPosition)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
