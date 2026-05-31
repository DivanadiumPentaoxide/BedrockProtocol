// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/level/map/ClientboundMapItemDataType.hpp"
#include "sculk/protocol/codec/level/map/MapDecoration.hpp"
#include "sculk/protocol/codec/level/map/MapTrackedActorUniqueId.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::inline abi_v975 {

class ClientboundMapItemDataPacket : public IPacket {
public:
    std::int64_t                         mMapId{};
    ClientboundMapItemDataType           mTypeFlag{};
    std::uint8_t                         mDimension{};
    bool                                 mIsLockedMap{};
    BlockPos                             mMapOrigin{};
    std::vector<std::int64_t>            mMapEntries{};
    std::uint8_t                         mScale{};
    std::vector<MapTrackedActorUniqueId> mTrackedActors{};
    std::vector<MapDecoration>           mDecorationList{};
    std::int32_t                         mTextureWidth{};
    std::int32_t                         mTextureHeight{};
    std::int32_t                         mXTexCoordinate{};
    std::int32_t                         mYTexCoordinate{};
    std::vector<std::uint32_t>           mPixels{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::inline abi_v975

SCULK_PROTOCOL_PACKET_FORMATTER(ClientboundMapItemDataPacket)