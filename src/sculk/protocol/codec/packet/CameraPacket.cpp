// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CameraPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CameraPacket::getId() const noexcept { return MinecraftPacketIds::Camera; }

std::string_view CameraPacket::getName() const noexcept { return "CameraPacket"; }

void CameraPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mCameraId);
    stream.writeVarInt64(mTargetPlayerId);
}

Result<> CameraPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mCameraId));
    return stream.readVarInt64(mTargetPlayerId);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CameraPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mCameraId), SCULK_FORMAT_FIELD(mTargetPlayerId));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
