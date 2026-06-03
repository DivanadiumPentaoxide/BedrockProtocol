// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AddItemActorPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AddItemActorPacket::getId() const noexcept { return MinecraftPacketIds::AddItemActor; }

std::string_view AddItemActorPacket::getName() const noexcept { return "AddItemActorPacket"; }

void AddItemActorPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mUniqueId);
    stream.writeUnsignedVarInt64(mRuntimeId);
    mItem.write(stream);
    mPos.write(stream);
    mVelocity.write(stream);
    mMetaData.write(stream);
    stream.writeBool(mIsFromFishing);
}

Result<> AddItemActorPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mUniqueId));
    _SCULK_READ(stream.readUnsignedVarInt64(mRuntimeId));
    _SCULK_READ(mItem.read(stream));
    _SCULK_READ(mPos.read(stream));
    _SCULK_READ(mVelocity.read(stream));
    _SCULK_READ(mMetaData.read(stream));
    return stream.readBool(mIsFromFishing);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AddItemActorPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mRuntimeId),
        SCULK_FORMAT_FIELD(mUniqueId),
        SCULK_FORMAT_FIELD(mItem),
        SCULK_FORMAT_FIELD(mPos),
        SCULK_FORMAT_FIELD(mVelocity),
        SCULK_FORMAT_FIELD(mMetaData),
        SCULK_FORMAT_FIELD(mIsFromFishing)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
