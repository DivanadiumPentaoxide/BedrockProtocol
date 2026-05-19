// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/player/LocatorBar.hpp"

namespace sculk::protocol::inline abi_v975 {

void LocatorBarWorldPosition::write(BinaryStream& stream) const {
    mPosition.write(stream);
    stream.writeVarInt(mDimensionType);
}

Result<> LocatorBarWorldPosition::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPosition.read(stream));
    return stream.readVarInt(mDimensionType);
}

void LocatorBarWaypointHandle::write(BinaryStream& stream) const { mUUID.write(stream); }

Result<> LocatorBarWaypointHandle::read(ReadOnlyBinaryStream& stream) { return mUUID.read(stream); }

void LocatorBarServerWaypointPayload::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(mUpdateFlag);
    stream.writeOptional(mIsVisible, &BinaryStream::writeBool);
    stream.writeOptional(mWorldPosition, &LocatorBarWorldPosition::write);
    stream.writeOptional(mTexturePath, &BinaryStream::writeString);
    stream.writeOptional(mIconSize, &Vec2::write);
    stream.writeOptional(mColor, &BinaryStream::writeSignedInt);
    stream.writeOptional(mClientPositionAuthority, &BinaryStream::writeBool);
    stream.writeOptional(mActorUniqueId, &BinaryStream::writeVarInt64);
}

Result<> LocatorBarServerWaypointPayload::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt(mUpdateFlag));
    _SCULK_READ(stream.readOptional(mIsVisible, &ReadOnlyBinaryStream::readBool));
    _SCULK_READ(stream.readOptional(mWorldPosition, &LocatorBarWorldPosition::read));
    _SCULK_READ(stream.readOptional(mTexturePath, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readOptional(mIconSize, &Vec2::read));
    _SCULK_READ(stream.readOptional(mColor, &ReadOnlyBinaryStream::readSignedInt));
    _SCULK_READ(stream.readOptional(mClientPositionAuthority, &ReadOnlyBinaryStream::readBool));
    return stream.readOptional(mActorUniqueId, &ReadOnlyBinaryStream::readVarInt64);
}

void LocatorBarWaypointPayload::write(BinaryStream& stream) const {
    mGroupHandle.write(stream);
    mServerWaypointPayload.write(stream);
    stream.writeEnum(mActionFlag, &BinaryStream::writeUnsignedVarInt);
}

Result<> LocatorBarWaypointPayload::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mGroupHandle.read(stream));
    _SCULK_READ(mServerWaypointPayload.read(stream));
    return stream.readEnum(mActionFlag, &ReadOnlyBinaryStream::readUnsignedVarInt);
}

} // namespace sculk::protocol::inline abi_v975
