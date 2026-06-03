// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PositionTrackingDBServerBroadcastPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PositionTrackingDBServerBroadcastPacket::getId() const noexcept {
    return MinecraftPacketIds::PositionTrackingDBServerBroadcast;
}

std::string_view PositionTrackingDBServerBroadcastPacket::getName() const noexcept {
    return "PositionTrackingDBServerBroadcastPacket";
}

void PositionTrackingDBServerBroadcastPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mAction, &BinaryStream::writeByte);
    stream.writeVarInt(mPositionTrackingId);
    mPositionTrackingData.write(stream);
}

Result<> PositionTrackingDBServerBroadcastPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mAction, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readVarInt(mPositionTrackingId));
    return mPositionTrackingData.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PositionTrackingDBServerBroadcastPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mAction),
        SCULK_FORMAT_FIELD(mPositionTrackingId),
        SCULK_FORMAT_FIELD(mPositionTrackingData)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
