// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerUpdateEntityOverridesPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerUpdateEntityOverridesPacket::getId() const noexcept {
    return MinecraftPacketIds::PlayerUpdateEntityOverrides;
}

std::string_view PlayerUpdateEntityOverridesPacket::getName() const noexcept {
    return "PlayerUpdateEntityOverridesPacket";
}

void PlayerUpdateEntityOverridesPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mId);
    stream.writeUnsignedVarInt(mPropertyIndex);
    stream.writeEnum(mUpdateType, &BinaryStream::writeByte);
    std::visit(
        Overload{
            [&](std::monostate) {},
            [&](std::int32_t value) { stream.writeSignedInt(value); },
            [&](float value) { stream.writeFloat(value); },
        },
        mValue
    );
}

Result<> PlayerUpdateEntityOverridesPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mId));
    _SCULK_READ(stream.readUnsignedVarInt(mPropertyIndex));
    _SCULK_READ(stream.readEnum(mUpdateType, &ReadOnlyBinaryStream::readByte));
    switch (mUpdateType) {
    case UpdateType::SetIntOverride: {
        std::int32_t value{};
        _SCULK_READ(stream.readSignedInt(value));
        mValue = value;
        break;
    }
    case UpdateType::SetFloatOverride: {
        float value{};
        _SCULK_READ(stream.readFloat(value));
        mValue = value;
        break;
    }
    default:
        mValue = std::monostate{};
        break;
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerUpdateEntityOverridesPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mValue),
        SCULK_FORMAT_FIELD(mUpdateType),
        SCULK_FORMAT_FIELD(mId),
        SCULK_FORMAT_FIELD(mPropertyIndex)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
