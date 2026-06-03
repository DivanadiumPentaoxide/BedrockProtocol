// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ChangeMobPropertyPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ChangeMobPropertyPacket::getId() const noexcept { return MinecraftPacketIds::ChangeMobProperty; }

std::string_view ChangeMobPropertyPacket::getName() const noexcept { return "ChangeMobPropertyPacket"; }

void ChangeMobPropertyPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mActorUniqueId);
    stream.writeString(mPropertyName);
    stream.writeBool(mBoolVaue);
    stream.writeString(mStringValue);
    stream.writeVarInt(mIntValue);
    stream.writeFloat(mFloatValue);
}

Result<> ChangeMobPropertyPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mActorUniqueId));
    _SCULK_READ(stream.readString(mPropertyName));
    _SCULK_READ(stream.readBool(mBoolVaue));
    _SCULK_READ(stream.readString(mStringValue));
    _SCULK_READ(stream.readVarInt(mIntValue));
    return stream.readFloat(mFloatValue);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ChangeMobPropertyPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorUniqueId),
        SCULK_FORMAT_FIELD(mPropertyName),
        SCULK_FORMAT_FIELD(mBoolVaue),
        SCULK_FORMAT_FIELD(mStringValue),
        SCULK_FORMAT_FIELD(mIntValue),
        SCULK_FORMAT_FIELD(mFloatValue)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
