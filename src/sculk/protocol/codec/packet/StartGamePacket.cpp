// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/StartGamePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif
#include "sculk/protocol/codec/nbt/TagType.hpp"
#include "sculk/protocol/codec/nbt/TagVariant.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds StartGamePacket::getId() const noexcept { return MinecraftPacketIds::StartGame; }

std::string_view StartGamePacket::getName() const noexcept { return "StartGamePacket"; }

void StartGamePacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mActorUniqueId);
    stream.writeUnsignedVarInt64(mActorRuntimeId);
    stream.writeEnum(mGameType, &BinaryStream::writeVarInt);
    mPosition.write(stream);
    mRotation.write(stream);
    mLevelSettings.write(stream);
    stream.writeString(mLevelId);
    stream.writeString(mLevelName);
    stream.writeString(mTemplateContentIdentity);
    stream.writeBool(mIsTrial);
    mMovementSettings.write(stream);
    stream.writeUnsignedInt64(mCurrentTime);
    stream.writeVarInt(mEnchantmentSeed);
    stream.writeArray(mBlockProperties, &BlockProperty::write);
    stream.writeString(mMultiPlayerCorrelationId);
    stream.writeBool(mEnableItemStackNetManager);
    stream.writeString(mServerVersion);
    mPlayerPropertyData.write(stream);
    stream.writeUnsignedInt64(mServerBlockCheckSum);
    mWorldTemplateId.write(stream);
    stream.writeBool(mClientGen);
    stream.writeBool(mEnableHashId);
    stream.writeBool(mServerAuthSound);
    stream.writeOptional(mServerConfigurationJoinInfo, &ServerConfigurationJoinInfo::write);
    mServerTelemetryData.write(stream);
}

Result<> StartGamePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mActorUniqueId));
    _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    _SCULK_READ(stream.readEnum(mGameType, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(mRotation.read(stream));
    _SCULK_READ(mLevelSettings.read(stream));
    _SCULK_READ(stream.readString(mLevelId));
    _SCULK_READ(stream.readString(mLevelName));
    _SCULK_READ(stream.readString(mTemplateContentIdentity));
    _SCULK_READ(stream.readBool(mIsTrial));
    _SCULK_READ(mMovementSettings.read(stream));
    _SCULK_READ(stream.readUnsignedInt64(mCurrentTime));
    _SCULK_READ(stream.readVarInt(mEnchantmentSeed));
    _SCULK_READ(stream.readArray(mBlockProperties, &BlockProperty::read));
    _SCULK_READ(stream.readString(mMultiPlayerCorrelationId));
    _SCULK_READ(stream.readBool(mEnableItemStackNetManager));
    _SCULK_READ(stream.readString(mServerVersion));
    _SCULK_READ(mPlayerPropertyData.read(stream));
    _SCULK_READ(stream.readUnsignedInt64(mServerBlockCheckSum));
    _SCULK_READ(mWorldTemplateId.read(stream));
    _SCULK_READ(stream.readBool(mClientGen));
    _SCULK_READ(stream.readBool(mEnableHashId));
    _SCULK_READ(stream.readBool(mServerAuthSound));
    _SCULK_READ(stream.readOptional(mServerConfigurationJoinInfo, &ServerConfigurationJoinInfo::read));
    return mServerTelemetryData.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string StartGamePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorUniqueId),
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mGameType),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mRotation),
        SCULK_FORMAT_FIELD(mLevelSettings),
        SCULK_FORMAT_FIELD(mLevelId),
        SCULK_FORMAT_FIELD(mLevelName),
        SCULK_FORMAT_FIELD(mTemplateContentIdentity),
        SCULK_FORMAT_FIELD(mIsTrial),
        SCULK_FORMAT_FIELD(mMovementSettings),
        SCULK_FORMAT_FIELD(mCurrentTime),
        SCULK_FORMAT_FIELD(mEnchantmentSeed),
        SCULK_FORMAT_FIELD(mBlockProperties),
        SCULK_FORMAT_FIELD(mMultiPlayerCorrelationId),
        SCULK_FORMAT_FIELD(mEnableItemStackNetManager),
        SCULK_FORMAT_FIELD(mServerVersion),
        SCULK_FORMAT_FIELD(mPlayerPropertyData),
        SCULK_FORMAT_FIELD(mServerBlockCheckSum),
        SCULK_FORMAT_FIELD(mWorldTemplateId),
        SCULK_FORMAT_FIELD(mClientGen),
        SCULK_FORMAT_FIELD(mEnableHashId),
        SCULK_FORMAT_FIELD(mServerAuthSound),
        SCULK_FORMAT_FIELD(mServerConfigurationJoinInfo),
        SCULK_FORMAT_FIELD(mServerTelemetryData)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
