// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/math/Vec3.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class MoveActorAbsolutePacket : public IPacket {
public:
    enum class Flags : std::uint8_t {
        OnGround        = 1u << 0,
        Teleport        = 1u << 1,
        ForceMove       = 1u << 2,
        ForceCompletion = 1u << 3,
    };

public:
    std::uint64_t mActorRuntimeId{};
    Flags         mFlags{};
    Vec3          mPosition{};
    std::uint8_t  mRotationX{};
    std::uint8_t  mRotationY{};
    std::uint8_t  mRotationYHead{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

constexpr MoveActorAbsolutePacket::Flags
operator|(MoveActorAbsolutePacket::Flags lhs, MoveActorAbsolutePacket::Flags rhs) {
    return static_cast<MoveActorAbsolutePacket::Flags>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}
constexpr MoveActorAbsolutePacket::Flags
operator&(MoveActorAbsolutePacket::Flags lhs, MoveActorAbsolutePacket::Flags rhs) {
    return static_cast<MoveActorAbsolutePacket::Flags>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(MoveActorAbsolutePacket)