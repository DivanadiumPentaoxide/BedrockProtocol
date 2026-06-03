// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/LevelSoundEventPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds LevelSoundEventPacket::getId() const noexcept { return MinecraftPacketIds::LevelSoundEvent; }

std::string_view LevelSoundEventPacket::getName() const noexcept { return "LevelSoundEventPacket"; }

void LevelSoundEventPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mEventId, &BinaryStream::writeUnsignedVarInt);
    mPosition.write(stream);
    stream.writeVarInt(mData);
    stream.writeString(mActorIdentifier);
    stream.writeBool(mIsBabyMob);
    stream.writeBool(mIsGlobal);
    stream.writeSignedInt64(mActorUniqueId);
    stream.writeOptional(mFireAtPosition, &Vec3::write);
}

Result<> LevelSoundEventPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mEventId, &ReadOnlyBinaryStream::readUnsignedVarInt));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readVarInt(mData));
    _SCULK_READ(stream.readString(mActorIdentifier));
    _SCULK_READ(stream.readBool(mIsBabyMob));
    _SCULK_READ(stream.readBool(mIsGlobal));
    _SCULK_READ(stream.readSignedInt64(mActorUniqueId));
    return stream.readOptional(mFireAtPosition, &Vec3::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LevelSoundEventPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mEventId),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mData),
        SCULK_FORMAT_FIELD(mActorIdentifier),
        SCULK_FORMAT_FIELD(mIsBabyMob),
        SCULK_FORMAT_FIELD(mIsGlobal),
        SCULK_FORMAT_FIELD(mFireAtPosition)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
