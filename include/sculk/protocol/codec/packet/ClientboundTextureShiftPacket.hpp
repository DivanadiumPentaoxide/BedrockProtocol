// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include <vector>

namespace sculk::protocol::inline abi_v975 {

class ClientboundTextureShiftPacket : public IPacket {
public:
    enum class ActionType : std::uint8_t {
        Invalid    = 0,
        Initialize = 1,
        Start      = 2,
        SetEnabled = 3,
        Sync       = 4,
    };

public:
    ActionType               mActionType{};
    std::string              mCollectionName{};
    std::string              mFromStep{};
    std::string              mToStep{};
    std::vector<std::string> mAllSteps{};
    std::uint64_t            mCurrentLengthInTicks{};
    std::uint64_t            mTotalLengthInTicks{};
    bool                     mIsEnabled{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::inline abi_v975

SCULK_PROTOCOL_PACKET_FORMATTER(ClientboundTextureShiftPacket)