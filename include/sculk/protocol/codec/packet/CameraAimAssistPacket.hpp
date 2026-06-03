// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/math/Vec2.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class CameraAimAssistPacket : public IPacket {
public:
    enum class TargetMode : std::uint8_t {
        Angle    = 0,
        Distance = 1,
    };

    enum class Action : std::uint8_t {
        Set   = 0,
        Clear = 1,
    };

public:
    std::string mPresetId{};
    Vec2        mViewAngle{};
    float       mDistance{};
    TargetMode  mTargetMode{};
    Action      mAction{};
    bool        mShowDebugRender{true};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(CameraAimAssistPacket)