// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/MetaData.hpp"
#include "sculk/protocol/codec/inventory/item/NetworkItemStackDescriptor.hpp"
#include "sculk/protocol/codec/math/Vec3.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::inline abi_v975 {

class AddItemActorPacket : public IPacket {
public:
    uint64_t                   mRuntimeId{};
    int64_t                    mUniqueId{};
    NetworkItemStackDescriptor mItem{};
    Vec3                       mPos{};
    Vec3                       mVelocity{};
    MetaData                   mMetaData{};
    bool                       mIsFromFishing{};

public:
    MinecraftPacketIds getId() const noexcept override;

    std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::inline abi_v975

SCULK_PROTOCOL_PACKET_FORMATTER(AddItemActorPacket)