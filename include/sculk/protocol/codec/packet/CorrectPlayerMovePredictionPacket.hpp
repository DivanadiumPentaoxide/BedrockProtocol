// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/math/Vec2.hpp"
#include "sculk/protocol/codec/math/Vec3.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include <optional>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class CorrectPlayerMovePredictionPacket : public IPacket {
public:
    enum class RewindType : std::uint8_t {
        Player  = 0,
        Vehicle = 1,
    };

public:
    RewindType           mPredictionType{};
    Vec3                 mPos{};
    Vec3                 mPosDelta{};
    Vec2                 mVehiclePrediction{};
    std::optional<float> mVehicleAngularVelocity{};
    bool                 mOnGround{};
    std::uint64_t        mPlayerInputTick{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(CorrectPlayerMovePredictionPacket)