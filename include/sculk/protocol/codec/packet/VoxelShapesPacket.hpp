// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/level/block/VoxelShapes.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::inline abi_v975 {

class VoxelShapesPacket : public IPacket {
public:
    struct RegistryHandle {
        std::string   mName{};
        std::uint16_t mValue{};

        void write(BinaryStream& stream) const;

        [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
    };

public:
    std::vector<SerializableVoxelShape> mShapes{};
    std::vector<RegistryHandle>         mRegistryHandles{};
    std::uint16_t                       mCustomShapeCount{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::inline abi_v975

SCULK_PROTOCOL_PACKET_FORMATTER(VoxelShapesPacket)