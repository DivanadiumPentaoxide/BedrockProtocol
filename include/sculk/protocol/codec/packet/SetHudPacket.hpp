// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class SetHudPacket : public IPacket {
public:
    enum class HudElement : std::int32_t {
        PaperDoll     = 0,
        Armor         = 1,
        ToolTips      = 2,
        TouchControls = 3,
        Crosshair     = 4,
        HotBar        = 5,
        Health        = 6,
        ProgressBar   = 7,
        Hunger        = 8,
        AirBubbles    = 9,
        HorseHealth   = 10,
        StatusEffects = 11,
        ItemText      = 12,
        Count         = 13,
    };

    enum class HudVisibility : std::int32_t {
        Hide  = 0,
        Reset = 1,
        Count = 2,
    };

public:
    std::vector<HudElement> mHudElements{};
    HudVisibility           mHudVisibility{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(SetHudPacket)