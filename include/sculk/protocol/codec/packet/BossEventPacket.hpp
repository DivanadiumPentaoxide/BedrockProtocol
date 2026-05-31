// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::inline abi_v975 {

class BossEventPacket : public IPacket {
public:
    enum class EventType : std::uint32_t {
        Add              = 0,
        PlayerAdded      = 1,
        Remove           = 2,
        PlayerRemoved    = 3,
        UpdatePercent    = 4,
        UpdateName       = 5,
        UpdateProperties = 6,
        UpdateStyle      = 7,
        Query            = 8,
    };

public:
    std::int64_t  mTargetActor{};
    std::int64_t  mPlayer{};
    EventType     mType{};
    std::string   mName{};
    std::string   mFilteredName{};
    float         mPercentage{};
    std::uint16_t mDarkenScreen{};
    std::uint32_t mColor{};
    std::uint32_t mOverlay{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::inline abi_v975

SCULK_PROTOCOL_PACKET_FORMATTER(BossEventPacket)