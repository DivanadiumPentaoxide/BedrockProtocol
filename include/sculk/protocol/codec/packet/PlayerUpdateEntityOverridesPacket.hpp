// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include <variant>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class UpdateType : std::uint8_t {
    ClearOverrides   = 0,
    RemoveOverride   = 1,
    SetIntOverride   = 2,
    SetFloatOverride = 3,
};

class PlayerUpdateEntityOverridesPacket : public IPacket {
public:
    std::variant<std::monostate, std::int32_t, float> mValue{};
    UpdateType                                        mUpdateType{};
    std::int64_t                                      mId{};
    std::uint32_t                                     mPropertyIndex{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(PlayerUpdateEntityOverridesPacket)