// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/ActorEvent.hpp"
#include "sculk/protocol/codec/actor/ActorLink.hpp"
#include "sculk/protocol/codec/actor/MetaData.hpp"
#include "sculk/protocol/codec/actor/PropertySyncData.hpp"
#include "sculk/protocol/codec/actor/SyncedAttribute.hpp"
#include "sculk/protocol/codec/math/Vec2.hpp"
#include "sculk/protocol/codec/math/Vec3.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class AddActorPacket : public IPacket {
public:
    std::uint64_t                mActorRuntimeId{};
    std::int64_t                 mActorUniqueId{};
    std::string                  mIdentifier{};
    std::vector<ActorLink>       mActorLinks{};
    Vec3                         mPosition{};
    Vec3                         mVelocity{};
    Vec2                         mRotation{};
    float                        mYHeadRotation{};
    float                        mYBodyRotation{};
    MetaData                     mMetaData{};
    std::vector<SyncedAttribute> mAttributes{};
    PropertySyncData             mSynchedProperties{};

public:
    MinecraftPacketIds getId() const noexcept override;

    std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(AddActorPacket)