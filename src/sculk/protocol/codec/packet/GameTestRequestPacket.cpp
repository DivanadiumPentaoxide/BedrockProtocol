// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/GameTestRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds GameTestRequestPacket::getId() const noexcept { return MinecraftPacketIds::GameTestRequest; }

std::string_view GameTestRequestPacket::getName() const noexcept { return "GameTestRequestPacket"; }

void GameTestRequestPacket::write(BinaryStream& stream) const {
    stream.writeVarInt(mMaxTestsPerBatch);
    stream.writeVarInt(mRepeatCount);
    stream.writeByte(mRotation);
    stream.writeBool(mStopOnFailed);
    mTestPos.write(stream);
    stream.writeVarInt(mTestsPerRow);
    stream.writeString(mTestName);
}

Result<> GameTestRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mMaxTestsPerBatch));
    _SCULK_READ(stream.readVarInt(mRepeatCount));
    _SCULK_READ(stream.readByte(mRotation));
    _SCULK_READ(stream.readBool(mStopOnFailed));
    _SCULK_READ(mTestPos.read(stream));
    _SCULK_READ(stream.readVarInt(mTestsPerRow));
    return stream.readString(mTestName);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string GameTestRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mMaxTestsPerBatch),
        SCULK_FORMAT_FIELD(mRepeatCount),
        SCULK_FORMAT_FIELD(mRotation),
        SCULK_FORMAT_FIELD(mStopOnFailed),
        SCULK_FORMAT_FIELD(mTestPos),
        SCULK_FORMAT_FIELD(mTestsPerRow),
        SCULK_FORMAT_FIELD(mTestName)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
