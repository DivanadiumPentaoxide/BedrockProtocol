// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/command/CommandOriginData.hpp"
#include "sculk/protocol/codec/packet/IPacket.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class CommandOutputPacket : public IPacket {
public:
    enum class Type {
        None       = 0,
        LastOutput = 1,
        Silent     = 2,
        AllOutput  = 3,
        DataSet    = 4,
    };

    struct OutputMessage {
        bool                     mSuccess{};
        std::string              mMessageId{};
        std::vector<std::string> mParameters{};
    };

public:
    CommandOriginData          mOriginData{};
    Type                       mOutputType{};
    std::uint32_t              mSuccessCount{};
    std::vector<OutputMessage> mOutputMessages{};
    std::string                mDataSet{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(CommandOutputPacket)