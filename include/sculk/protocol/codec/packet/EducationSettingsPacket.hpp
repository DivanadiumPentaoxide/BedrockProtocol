// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include <optional>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class EducationSettingsPacket : public IPacket {
public:
    struct AgentCapabilities {
        std::optional<bool> mCanModifyBlocks{};

        void write(BinaryStream& stream) const;

        [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
    };

    struct ExternalLinkSettings {
        std::string mUrl{};
        std::string mDisplayName{};

        void write(BinaryStream& stream) const;

        [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
    };

public:
    std::string                         mCodeBuilderDefaultUrl{};
    std::string                         mCodeBuilderTitle{};
    bool                                mCanResizeCodeBuilder{};
    bool                                mDisableLegacyTitleBar{};
    std::string                         mPostProcessFilter{};
    std::string                         mScreenshotBorderResourcePath{};
    std::optional<AgentCapabilities>    mAgentCapabilities{};
    std::optional<std::string>          mCodeBuilderOverrideUrl{};
    bool                                mAlwaysFalse{};
    std::optional<ExternalLinkSettings> mExternalLinkSettings{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(EducationSettingsPacket)