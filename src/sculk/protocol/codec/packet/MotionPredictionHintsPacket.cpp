// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MotionPredictionHintsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds MotionPredictionHintsPacket::getId() const noexcept {
    return MinecraftPacketIds::MotionPredictionHints;
}

std::string_view MotionPredictionHintsPacket::getName() const noexcept { return "MotionPredictionHintsPacket"; }

void MotionPredictionHintsPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    mMotion.write(stream);
    stream.writeBool(mOnGround);
}

Result<> MotionPredictionHintsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(mMotion.read(stream));
    return stream.readBool(mOnGround);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MotionPredictionHintsPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mMotion),
        SCULK_FORMAT_FIELD(mOnGround)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
