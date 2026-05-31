// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::inline abi_v975 {

class BookEditPacket : public IPacket {
public:
    enum class Action : std::uint32_t {
        ReplacePage = 0,
        AddPage     = 1,
        DeletePage  = 2,
        SwapPages   = 3,
        Finalize    = 4,
    };

public:
    Action       mAction{};
    std::int32_t mBookSlot{};
    std::int32_t mPageIndexA{};
    std::int32_t mPageIndexB{};
    std::string  mTextA{};
    std::string  mTextB{};
    std::string  mXuid{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::inline abi_v975

SCULK_PROTOCOL_PACKET_FORMATTER(BookEditPacket)