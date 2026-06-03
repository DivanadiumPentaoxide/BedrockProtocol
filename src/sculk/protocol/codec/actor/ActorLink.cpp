// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/ActorLink.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ActorLink::write(BinaryStream& stream) const {
    stream.writeVarInt64(mTargetAUniqueId);
    stream.writeVarInt64(mTargetBUniqueId);
    stream.writeEnum(mType, &BinaryStream::writeByte);
    stream.writeBool(mImmediate);
    stream.writeBool(mPassengerInitiated);
    stream.writeFloat(mVehicleAngularVelocity);
}

Result<> ActorLink::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mTargetAUniqueId));
    _SCULK_READ(stream.readVarInt64(mTargetBUniqueId));
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readBool(mImmediate));
    _SCULK_READ(stream.readBool(mPassengerInitiated));
    return stream.readFloat(mVehicleAngularVelocity);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
