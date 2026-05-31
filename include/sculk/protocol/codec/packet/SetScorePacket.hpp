// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::inline abi_v975 {

class SetScorePacket : public IPacket {
public:
    enum class PacketType : std::uint8_t {
        Change = 0,
        Remove = 1,
    };

    enum class IdentityType : std::uint8_t {
        Invalid    = 0,
        Player     = 1,
        Entity     = 2,
        FakePlayer = 3,
    };

    struct ScoreInfo {
        std::int64_t mScoreboardId{};
        std::string  mObjectiveName{};
        std::int32_t mScoreValue{};
        IdentityType mIdentityType{};
        std::int64_t mActorUniqueId{};
        std::string  mFakePlayerName{};
    };

public:
    PacketType             mPacketType{};
    std::vector<ScoreInfo> mScoresInfo{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::inline abi_v975

SCULK_PROTOCOL_PACKET_FORMATTER(SetScorePacket)