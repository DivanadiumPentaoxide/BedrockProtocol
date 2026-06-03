// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MobEffectPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds MobEffectPacket::getId() const noexcept { return MinecraftPacketIds::MobEffect; }

std::string_view MobEffectPacket::getName() const noexcept { return "MobEffectPacket"; }

void MobEffectPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeEnum(mEventId, &BinaryStream::writeByte);
    stream.writeVarInt(mEffectId);
    stream.writeVarInt(mEffectAmplifier);
    stream.writeBool(mShowParticles);
    stream.writeVarInt(mEffectDurationTicks);
    stream.writeUnsignedVarInt64(mTick);
    stream.writeBool(mAmbient);
}

Result<> MobEffectPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(stream.readEnum(mEventId, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readVarInt(mEffectId));
    _SCULK_READ(stream.readVarInt(mEffectAmplifier));
    _SCULK_READ(stream.readBool(mShowParticles));
    _SCULK_READ(stream.readVarInt(mEffectDurationTicks));
    _SCULK_READ(stream.readUnsignedVarInt64(mTick));
    return stream.readBool(mAmbient);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MobEffectPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mEventId),
        SCULK_FORMAT_FIELD(mEffectId),
        SCULK_FORMAT_FIELD(mEffectAmplifier),
        SCULK_FORMAT_FIELD(mShowParticles),
        SCULK_FORMAT_FIELD(mEffectDurationTicks),
        SCULK_FORMAT_FIELD(mTick),
        SCULK_FORMAT_FIELD(mAmbient)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
