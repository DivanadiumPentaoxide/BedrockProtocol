// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/player/GameType.hpp"
#include "sculk/protocol/codec/actor/player/SyncedPlayerMovementSettings.hpp"
#include "sculk/protocol/codec/actor/player/UUID.hpp"
#include "sculk/protocol/codec/level/LevelSettings.hpp"
#include "sculk/protocol/codec/level/ServerConfiguration.hpp"
#include "sculk/protocol/codec/level/ServerTelemetryData.hpp"
#include "sculk/protocol/codec/level/block/BlockProperty.hpp"
#include "sculk/protocol/codec/math/Vec2.hpp"
#include "sculk/protocol/codec/math/Vec3.hpp"
#include "sculk/protocol/codec/nbt/TagVariant.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class StartGamePacket : public IPacket {
public:
    std::int64_t                               mActorUniqueId{};
    std::uint64_t                              mActorRuntimeId{};
    GameType                                   mGameType{};
    Vec3                                       mPosition{};
    Vec2                                       mRotation{};
    LevelSettings                              mLevelSettings{};
    std::string                                mLevelId{};
    std::string                                mLevelName{};
    std::string                                mTemplateContentIdentity{};
    bool                                       mIsTrial{};
    SyncedPlayerMovementSettings               mMovementSettings{};
    std::uint64_t                              mCurrentTime{};
    int                                        mEnchantmentSeed{};
    std::vector<BlockProperty>                 mBlockProperties{};
    std::string                                mMultiPlayerCorrelationId{};
    bool                                       mEnableItemStackNetManager{};
    std::string                                mServerVersion{};
    CompoundTag                                mPlayerPropertyData{};
    std::uint64_t                              mServerBlockCheckSum{};
    UUID                                       mWorldTemplateId{};
    bool                                       mClientGen{};
    bool                                       mEnableHashId{};
    bool                                       mServerAuthSound{};
    std::optional<ServerConfigurationJoinInfo> mServerConfigurationJoinInfo{};
    ServerTelemetryData                        mServerTelemetryData{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(StartGamePacket)