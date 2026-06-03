// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AddPlayerPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AddPlayerPacket::getId() const noexcept { return MinecraftPacketIds::AddPlayer; }

std::string_view AddPlayerPacket::getName() const noexcept { return "AddPlayerPacket"; }

void AddPlayerPacket::write(BinaryStream& stream) const {
    mUuid.write(stream);
    stream.writeString(mName);
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeString(mPlatformOnlineId);
    mPos.write(stream);
    mVelocity.write(stream);
    mRot.write(stream);
    stream.writeFloat(mYHeadRot);
    mCarriedItem.write(stream);
    stream.writeEnum(mGameType, &BinaryStream::writeVarInt);
    mMetaData.write(stream);
    mSynchedProperties.write(stream);
    mAbilities.write(stream);
    stream.writeArray(mActorLinks, &ActorLink::write);
    stream.writeString(mDeviceId);
    stream.writeUnsignedInt(mBuildPlatform);
}

Result<> AddPlayerPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mUuid.read(stream));
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(stream.readString(mPlatformOnlineId));
    _SCULK_READ(mPos.read(stream));
    _SCULK_READ(mVelocity.read(stream));
    _SCULK_READ(mRot.read(stream));
    _SCULK_READ(stream.readFloat(mYHeadRot));
    _SCULK_READ(mCarriedItem.read(stream));
    _SCULK_READ(stream.readEnum(mGameType, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(mMetaData.read(stream));
    _SCULK_READ(mSynchedProperties.read(stream));
    _SCULK_READ(mAbilities.read(stream));
    _SCULK_READ(stream.readArray(mActorLinks, &ActorLink::read));
    _SCULK_READ(stream.readString(mDeviceId));
    return stream.readUnsignedInt(mBuildPlatform);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AddPlayerPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mActorLinks),
        SCULK_FORMAT_FIELD(mName),
        SCULK_FORMAT_FIELD(mUuid),
        SCULK_FORMAT_FIELD(mPlatformOnlineId),
        SCULK_FORMAT_FIELD(mPos),
        SCULK_FORMAT_FIELD(mVelocity),
        SCULK_FORMAT_FIELD(mRot),
        SCULK_FORMAT_FIELD(mYHeadRot),
        SCULK_FORMAT_FIELD(mAbilities),
        SCULK_FORMAT_FIELD(mDeviceId),
        SCULK_FORMAT_FIELD(mBuildPlatform),
        SCULK_FORMAT_FIELD(mGameType),
        SCULK_FORMAT_FIELD(mCarriedItem),
        SCULK_FORMAT_FIELD(mMetaData),
        SCULK_FORMAT_FIELD(mSynchedProperties)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
