// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/LegacyTelemetryEventPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds LegacyTelemetryEventPacket::getId() const noexcept {
    return MinecraftPacketIds::LegacyTelemetryEvent;
}

std::string_view LegacyTelemetryEventPacket::getName() const noexcept { return "LegacyTelemetryEventPacket"; }

void LegacyTelemetryEventPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mActorUniqueId);
    stream.writeEnum(mEventType, &BinaryStream::writeVarInt);
    stream.writeByte(mUsePlayerId);
    stream.writeVariant(
        mEventData,
        Overload{
            [&](const Achievement& data) { stream.writeVarInt(static_cast<std::int32_t>(data.mAchievementId)); },
            [&](const Interaction& data) {
                stream.writeVarInt64(data.mInteractedEntityId);
                stream.writeVarInt(static_cast<std::int32_t>(data.mInteractionType));
                stream.writeVarInt(data.mInteractionActorType);
                stream.writeVarInt(data.mInteractionActorVariant);
                stream.writeByte(data.mInteractionActorColor);
            },
            [&](const PortalCreated& data) { stream.writeVarInt(data.mDimensionType); },
            [&](const PortalUsed& data) {
                stream.writeVarInt(data.mSourceDimensionType);
                stream.writeVarInt(data.mTargetDimensionType);
            },
            [&](const MobKilled& data) {
                stream.writeVarInt64(data.mKillerActorId);
                stream.writeVarInt64(data.mKilledActorId);
                stream.writeVarInt(static_cast<std::int32_t>(data.mKillerChildActorType));
                stream.writeVarInt(data.mDamageSource);
                stream.writeVarInt(data.mTradeTier);
                stream.writeString(data.mTraderName);
            },
            [&](const CauldronUsed& data) {
                stream.writeUnsignedVarInt(data.mContentsColor);
                stream.writeVarInt(data.mContentsType);
                stream.writeVarInt(data.mFillLevel);
            },
            [&](const PlayerDied& data) {
                stream.writeVarInt(data.mKillerId);
                stream.writeVarInt(data.mKillerVariant);
                stream.writeVarInt(data.mDamageSource);
                stream.writeBool(data.mDiedInRaid);
            },
            [&](const BossKilled& data) {
                stream.writeVarInt64(data.mBossUniqueId);
                stream.writeVarInt(data.mPartySize);
                stream.writeVarInt(data.mBossType);
            },
            [&](const SlashCommand& data) {
                stream.writeVarInt(data.mSuccessCount);
                stream.writeVarInt(data.mErrorCount);
                stream.writeString(data.mCommandName);
                stream.writeString(data.mErrorList);
            },
            [&](const MobBorn& data) {
                stream.writeEnum(data.mMobType, &BinaryStream::writeVarInt);
                stream.writeVarInt(data.mMobVariant);
                stream.writeByte(data.mColor);
            },
            [&](const POICauldronUsed& data) {
                stream.writeVarInt(static_cast<std::int32_t>(data.mInteractionType));
                stream.writeVarInt(data.mItemId);
            },
            [&](const ComposterUsed& data) {
                stream.writeVarInt(static_cast<std::int32_t>(data.mInteractionType));
                stream.writeVarInt(data.mItemId);
            },
            [&](const BellUsed& data) { stream.writeVarInt(data.mItemId); },
            [&](const ActorDefinition& data) { stream.writeString(data.mEventName); },
            [&](const RaidUpdate& data) {
                stream.writeVarInt(data.mCurrentWave);
                stream.writeVarInt(data.mTotalWaves);
                stream.writeBool(data.mSuccess);
            },
            [&](const TargetBlockHit& data) { stream.writeVarInt(data.mRedstoneLevel); },
            [&](const PiglinBarter& data) {
                stream.writeVarInt(data.mItemId);
                stream.writeBool(data.mWasTargetingBarteringPlayer);
            },
            [&](const PlayerWaxedOrUnwaxedCopper& data) { stream.writeVarInt(data.mBlockId); },
            [&](const CodeBuilderRuntimeAction& data) { stream.writeString(data.mCodeBuilderRuntimeAction); },
            [&](const CodeBuilderScoreboard& data) {
                stream.writeString(data.mObjectiveName);
                stream.writeVarInt(data.mScore);
            },
            [&](const ItemUsed& data) {
                stream.writeSignedShort(data.mItemId);
                stream.writeSignedInt(data.mItemAux);
                stream.writeSignedInt(data.mUseMethod);
                stream.writeSignedInt(data.mCount);
            },
            [&](const std::monostate&) {}
        }
    );
}

Result<> LegacyTelemetryEventPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mActorUniqueId));
    _SCULK_READ(stream.readEnum(mEventType, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readByte(mUsePlayerId));
    return stream.readVariant(
        mEventData,
        Overload{
            [&](Achievement& data) {
                std::int32_t achievementId{};
                _SCULK_READ(stream.readVarInt(achievementId));
                data.mAchievementId = static_cast<AchievementIDs>(achievementId);
                return Result<>{};
            },
            [&](Interaction& data) {
                _SCULK_READ(stream.readVarInt64(data.mInteractedEntityId));
                int interactionType{};
                _SCULK_READ(stream.readVarInt(interactionType));
                data.mInteractionType = static_cast<InteractionType>(interactionType);
                _SCULK_READ(stream.readVarInt(data.mInteractionActorType));
                _SCULK_READ(stream.readVarInt(data.mInteractionActorVariant));
                _SCULK_READ(stream.readByte(data.mInteractionActorColor));
                return Result<>{};
            },
            [&](PortalCreated& data) { return stream.readVarInt(data.mDimensionType); },
            [&](PortalUsed& data) {
                _SCULK_READ(stream.readVarInt(data.mSourceDimensionType));
                _SCULK_READ(stream.readVarInt(data.mTargetDimensionType));
                return Result<>{};
            },
            [&](MobKilled& data) {
                _SCULK_READ(stream.readVarInt64(data.mKillerActorId));
                _SCULK_READ(stream.readVarInt64(data.mKilledActorId));
                _SCULK_READ(stream.readEnum(data.mKillerChildActorType, &ReadOnlyBinaryStream::readVarInt));
                _SCULK_READ(stream.readVarInt(data.mDamageSource));
                _SCULK_READ(stream.readVarInt(data.mTradeTier));
                _SCULK_READ(stream.readString(data.mTraderName));
                return Result<>{};
            },
            [&](CauldronUsed& data) {
                _SCULK_READ(stream.readUnsignedVarInt(data.mContentsColor));
                _SCULK_READ(stream.readVarInt(data.mContentsType));
                _SCULK_READ(stream.readVarInt(data.mFillLevel));
                return Result<>{};
            },
            [&](PlayerDied& data) {
                _SCULK_READ(stream.readVarInt(data.mKillerId));
                _SCULK_READ(stream.readVarInt(data.mKillerVariant));
                _SCULK_READ(stream.readVarInt(data.mDamageSource));
                _SCULK_READ(stream.readBool(data.mDiedInRaid));
                return Result<>{};
            },
            [&](BossKilled& data) {
                _SCULK_READ(stream.readVarInt64(data.mBossUniqueId));
                _SCULK_READ(stream.readVarInt(data.mPartySize));
                _SCULK_READ(stream.readVarInt(data.mBossType));
                return Result<>{};
            },
            [&](SlashCommand& data) {
                _SCULK_READ(stream.readVarInt(data.mSuccessCount));
                _SCULK_READ(stream.readVarInt(data.mErrorCount));
                _SCULK_READ(stream.readString(data.mCommandName));
                _SCULK_READ(stream.readString(data.mErrorList));
                return Result<>{};
            },
            [&](MobBorn& data) {
                _SCULK_READ(stream.readEnum(data.mMobType, &ReadOnlyBinaryStream::readVarInt));
                _SCULK_READ(stream.readVarInt(data.mMobVariant));
                _SCULK_READ(stream.readByte(data.mColor));
                return Result<>{};
            },
            [&](POICauldronUsed& data) {
                int interactionType{};
                _SCULK_READ(stream.readVarInt(interactionType));
                data.mInteractionType = static_cast<POIBlockInteractionType>(interactionType);
                _SCULK_READ(stream.readVarInt(data.mItemId));
                return Result<>{};
            },
            [&](ComposterUsed& data) {
                int interactionType{};
                _SCULK_READ(stream.readVarInt(interactionType));
                data.mInteractionType = static_cast<POIBlockInteractionType>(interactionType);
                _SCULK_READ(stream.readVarInt(data.mItemId));
                return Result<>{};
            },
            [&](BellUsed& data) {
                _SCULK_READ(stream.readVarInt(data.mItemId));
                return Result<>{};
            },
            [&](ActorDefinition& data) { return stream.readString(data.mEventName); },
            [&](RaidUpdate& data) {
                _SCULK_READ(stream.readVarInt(data.mCurrentWave));
                _SCULK_READ(stream.readVarInt(data.mTotalWaves));
                _SCULK_READ(stream.readBool(data.mSuccess));
                return Result<>{};
            },
            [&](TargetBlockHit& data) { return stream.readVarInt(data.mRedstoneLevel); },
            [&](PiglinBarter& data) {
                _SCULK_READ(stream.readVarInt(data.mItemId));
                _SCULK_READ(stream.readBool(data.mWasTargetingBarteringPlayer));
                return Result<>{};
            },
            [&](PlayerWaxedOrUnwaxedCopper& data) { return stream.readVarInt(data.mBlockId); },
            [&](CodeBuilderRuntimeAction& data) { return stream.readString(data.mCodeBuilderRuntimeAction); },
            [&](CodeBuilderScoreboard& data) {
                _SCULK_READ(stream.readString(data.mObjectiveName));
                _SCULK_READ(stream.readVarInt(data.mScore));
                return Result<>{};
            },
            [&](ItemUsed& data) {
                _SCULK_READ(stream.readSignedShort(data.mItemId));
                _SCULK_READ(stream.readSignedInt(data.mItemAux));
                _SCULK_READ(stream.readSignedInt(data.mUseMethod));
                _SCULK_READ(stream.readSignedInt(data.mCount));
                return Result<>{};
            },
            [&](std::monostate&) { return Result<>{}; }
        }
    );
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LegacyTelemetryEventPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mActorUniqueId),
        SCULK_FORMAT_FIELD(mEventType),
        SCULK_FORMAT_FIELD(mUsePlayerId)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
