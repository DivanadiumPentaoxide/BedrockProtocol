// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/SyncWorldClocks.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void TimeMarkerData::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mId);
    stream.writeString(mName);
    stream.writeVarInt(mTime);
    stream.writeSignedInt(mPeriod);
}

Result<> TimeMarkerData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mId));
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readVarInt(mTime));
    return stream.readSignedInt(mPeriod);
}

void SyncWorldClockStateData::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mClockId);
    stream.writeVarInt(mTime);
    stream.writeBool(mIsPaused);
}

Result<> SyncWorldClockStateData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mClockId));
    _SCULK_READ(stream.readVarInt(mTime));
    return stream.readBool(mIsPaused);
}

void WorldClockData::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mId);
    stream.writeString(mName);
    stream.writeVarInt(mTime);
    stream.writeBool(mIsPaused);
    stream.writeArray(mTimeMarkers, &TimeMarkerData::write);
}

Result<> WorldClockData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mId));
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readVarInt(mTime));
    _SCULK_READ(stream.readBool(mIsPaused));
    return stream.readArray(mTimeMarkers, &TimeMarkerData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
