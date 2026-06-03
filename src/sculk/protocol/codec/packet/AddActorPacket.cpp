// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AddActorPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AddActorPacket::getId() const noexcept { return MinecraftPacketIds::AddActor; }

std::string_view AddActorPacket::getName() const noexcept { return "AddActorPacket"; }

void AddActorPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mActorUniqueId);
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeString(mIdentifier);
    mPosition.write(stream);
    mVelocity.write(stream);
    mRotation.write(stream);
    stream.writeFloat(mYHeadRotation);
    stream.writeFloat(mYBodyRotation);
    stream.writeArray(mAttributes, &SyncedAttribute::write);
    mMetaData.write(stream);
    mSynchedProperties.write(stream);
    stream.writeArray(mActorLinks, &ActorLink::write);
}

Result<> AddActorPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mActorUniqueId));
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(stream.readString(mIdentifier));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(mVelocity.read(stream));
    _SCULK_READ(mRotation.read(stream));
    _SCULK_READ(stream.readFloat(mYHeadRotation));
    _SCULK_READ(stream.readFloat(mYBodyRotation));
    _SCULK_READ(stream.readArray(mAttributes, &SyncedAttribute::read));
    _SCULK_READ(mMetaData.read(stream));
    _SCULK_READ(mSynchedProperties.read(stream));
    return stream.readArray(mActorLinks, &ActorLink::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AddActorPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mActorUniqueId),
        SCULK_FORMAT_FIELD(mIdentifier),
        SCULK_FORMAT_FIELD(mActorLinks),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mVelocity),
        SCULK_FORMAT_FIELD(mRotation),
        SCULK_FORMAT_FIELD(mYHeadRotation),
        SCULK_FORMAT_FIELD(mYBodyRotation),
        SCULK_FORMAT_FIELD(mMetaData),
        SCULK_FORMAT_FIELD(mAttributes),
        SCULK_FORMAT_FIELD(mSynchedProperties)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
