// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientboundTextureShiftPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientboundTextureShiftPacket::getId() const noexcept {
    return MinecraftPacketIds::ClientboundTextureShift;
}

std::string_view ClientboundTextureShiftPacket::getName() const noexcept { return "ClientboundTextureShiftPacket"; }

void ClientboundTextureShiftPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mActionType, &BinaryStream::writeByte);
    stream.writeString(mCollectionName);
    stream.writeString(mFromStep);
    stream.writeString(mToStep);
    stream.writeArray(mAllSteps, &BinaryStream::writeString);
    stream.writeUnsignedVarInt64(mCurrentLengthInTicks);
    stream.writeUnsignedVarInt64(mTotalLengthInTicks);
    stream.writeBool(mIsEnabled);
}

Result<> ClientboundTextureShiftPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mActionType, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readString(mCollectionName));
    _SCULK_READ(stream.readString(mFromStep));
    _SCULK_READ(stream.readString(mToStep));
    _SCULK_READ(stream.readArray(mAllSteps, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readUnsignedVarInt64(mCurrentLengthInTicks));
    _SCULK_READ(stream.readUnsignedVarInt64(mTotalLengthInTicks));
    return stream.readBool(mIsEnabled);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientboundTextureShiftPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActionType),
        SCULK_FORMAT_FIELD(mCollectionName),
        SCULK_FORMAT_FIELD(mFromStep),
        SCULK_FORMAT_FIELD(mToStep),
        SCULK_FORMAT_FIELD(mAllSteps),
        SCULK_FORMAT_FIELD(mCurrentLengthInTicks),
        SCULK_FORMAT_FIELD(mTotalLengthInTicks),
        SCULK_FORMAT_FIELD(mIsEnabled)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
