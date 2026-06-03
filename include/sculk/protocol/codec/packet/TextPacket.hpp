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

class TextPacket : public IPacket {
public:
    enum class Type : std::uint8_t {
        Raw                    = 0,
        Chat                   = 1,
        Translate              = 2,
        Popup                  = 3,
        JukeboxPopup           = 4,
        Tip                    = 5,
        SystemMessage          = 6,
        Whisper                = 7,
        Announcement           = 8,
        TextObjectWhisper      = 9,
        TextObject             = 10,
        TextObjectAnnouncement = 11,
    };

    struct MessageOnly {
        std::string mMessage{};
    };

    struct AuthorAndMessage {
        std::string mPlayerName{};
        std::string mMessage{};
    };

    struct MessageAndParams {
        std::string              mMessage{};
        std::vector<std::string> mParameters{};
    };

    using TextBodyVariant = std::variant<MessageOnly, AuthorAndMessage, MessageAndParams>;

public:
    bool            mLocalize{};
    Type            mType{};
    TextBodyVariant mBody{};
    std::string     mXuid{};
    std::string     mPlatformId{};
    std::string     mFilteredMessage{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(TextPacket)