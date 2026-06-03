// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/UpdateTradePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds UpdateTradePacket::getId() const noexcept { return MinecraftPacketIds::UpdateTrade; }

std::string_view UpdateTradePacket::getName() const noexcept { return "UpdateTradePacket"; }

void UpdateTradePacket::write(BinaryStream& stream) const {
    stream.writeEnum(mContainerId, &BinaryStream::writeByte);
    stream.writeEnum(mContainerType, &BinaryStream::writeByte);
    stream.writeVarInt(mSize);
    stream.writeVarInt(mTier);
    stream.writeVarInt64(mEntityUniqueId);
    stream.writeVarInt64(mLastTradingPlayer);
    stream.writeString(mDisplayName);
    stream.writeBool(mUseNewTradeScreen);
    stream.writeBool(mUseEconomyTrade);
    mOffers.write(stream);
}

Result<> UpdateTradePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mContainerId, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readEnum(mContainerType, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readVarInt(mSize));
    _SCULK_READ(stream.readVarInt(mTier));
    _SCULK_READ(stream.readVarInt64(mEntityUniqueId));
    _SCULK_READ(stream.readVarInt64(mLastTradingPlayer));
    _SCULK_READ(stream.readString(mDisplayName));
    _SCULK_READ(stream.readBool(mUseNewTradeScreen));
    _SCULK_READ(stream.readBool(mUseEconomyTrade));
    return mOffers.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string UpdateTradePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mContainerId),
        SCULK_FORMAT_FIELD(mContainerType),
        SCULK_FORMAT_FIELD(mSize),
        SCULK_FORMAT_FIELD(mTier),
        SCULK_FORMAT_FIELD(mEntityUniqueId),
        SCULK_FORMAT_FIELD(mLastTradingPlayer),
        SCULK_FORMAT_FIELD(mDisplayName),
        SCULK_FORMAT_FIELD(mOffers)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
