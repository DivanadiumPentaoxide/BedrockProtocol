// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ServerboundPackSettingChangePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ServerboundPackSettingChangePacket::getId() const noexcept {
    return MinecraftPacketIds::ServerboundPackSettingChange;
}

std::string_view ServerboundPackSettingChangePacket::getName() const noexcept {
    return "ServerboundPackSettingChangePacket";
}

void ServerboundPackSettingChangePacket::write(BinaryStream& stream) const {
    mPackId.write(stream);
    stream.writeByte(mPackSettingDataType);
    stream.writeString(mPackSettingName);
    stream.writeBool(mBoolValue);
    stream.writeFloat(mFloatValue);
    stream.writeString(mStringValue);
}

Result<> ServerboundPackSettingChangePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPackId.read(stream));
    _SCULK_READ(stream.readByte(mPackSettingDataType));
    _SCULK_READ(stream.readString(mPackSettingName));
    _SCULK_READ(stream.readBool(mBoolValue));
    _SCULK_READ(stream.readFloat(mFloatValue));
    return stream.readString(mStringValue);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ServerboundPackSettingChangePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPackId),
        SCULK_FORMAT_FIELD(mPackSettingDataType),
        SCULK_FORMAT_FIELD(mPackSettingName),
        SCULK_FORMAT_FIELD(mBoolValue),
        SCULK_FORMAT_FIELD(mFloatValue),
        SCULK_FORMAT_FIELD(mStringValue)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
