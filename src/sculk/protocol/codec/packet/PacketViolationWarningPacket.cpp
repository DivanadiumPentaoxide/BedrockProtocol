// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PacketViolationWarningPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PacketViolationWarningPacket::getId() const noexcept {
    return MinecraftPacketIds::PacketViolationWarning;
}

std::string_view PacketViolationWarningPacket::getName() const noexcept { return "PacketViolationWarningPacket"; }

void PacketViolationWarningPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mType, &BinaryStream::writeVarInt);
    stream.writeEnum(mSeverity, &BinaryStream::writeVarInt);
    stream.writeEnum(mPacketId, &BinaryStream::writeVarInt);
    stream.writeString(mContext);
}

Result<> PacketViolationWarningPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readEnum(mSeverity, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readEnum(mPacketId, &ReadOnlyBinaryStream::readVarInt));
    return stream.readString(mContext);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PacketViolationWarningPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mType),
        SCULK_FORMAT_FIELD(mSeverity),
        SCULK_FORMAT_FIELD(mPacketId),
        SCULK_FORMAT_FIELD(mContext)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
