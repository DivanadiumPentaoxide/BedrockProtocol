// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PositionTrackingDBClientRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PositionTrackingDBClientRequestPacket::getId() const noexcept {
    return MinecraftPacketIds::PositionTrackingDBClientRequest;
}

std::string_view PositionTrackingDBClientRequestPacket::getName() const noexcept {
    return "PositionTrackingDBClientRequestPacket";
}

void PositionTrackingDBClientRequestPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mAction, &BinaryStream::writeByte);
    stream.writeVarInt(mPositionTrackingId);
}

Result<> PositionTrackingDBClientRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mAction, &ReadOnlyBinaryStream::readByte));
    return stream.readVarInt(mPositionTrackingId);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PositionTrackingDBClientRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mAction), SCULK_FORMAT_FIELD(mPositionTrackingId));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
