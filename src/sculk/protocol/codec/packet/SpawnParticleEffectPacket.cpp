// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SpawnParticleEffectPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SpawnParticleEffectPacket::getId() const noexcept { return MinecraftPacketIds::SpawnParticleEffect; }

std::string_view SpawnParticleEffectPacket::getName() const noexcept { return "SpawnParticleEffectPacket"; }

void SpawnParticleEffectPacket::write(BinaryStream& stream) const {
    stream.writeByte(mDimensionId);
    stream.writeVarInt64(mActorUniqueId);
    mPosition.write(stream);
    stream.writeString(mEffectName);
    stream.writeOptional(mMolangVariableMap, &BinaryStream::writeString);
}

Result<> SpawnParticleEffectPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readByte(mDimensionId));
    _SCULK_READ(stream.readVarInt64(mActorUniqueId));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readString(mEffectName));
    return stream.readOptional(mMolangVariableMap, &ReadOnlyBinaryStream::readString);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SpawnParticleEffectPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mDimensionId),
        SCULK_FORMAT_FIELD(mActorUniqueId),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mEffectName),
        SCULK_FORMAT_FIELD(mMolangVariableMap)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
