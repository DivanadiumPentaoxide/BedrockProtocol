// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/MinecraftPacketIds.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include <format>
#if __has_include(<fmt/format.h>)
#define SCULK_PROTOCOL_HAS_FMTLIB
#include <fmt/format.h>
#endif
#endif

namespace sculk::protocol::inline abi_v975 {

class IPacket {
public:
    std::uint8_t mSenderSubClientId{};
    std::uint8_t mTargetSubClientId{};

public:
    IPacket() noexcept = default;

public:
    virtual ~IPacket() noexcept = default;

    [[nodiscard]] virtual MinecraftPacketIds getId() const noexcept = 0;

    [[nodiscard]] virtual std::string_view getName() const noexcept = 0;

    virtual void write(BinaryStream& stream) const = 0;

    [[nodiscard]] virtual Result<> read(ReadOnlyBinaryStream& stream) = 0;

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
    [[nodiscard]] virtual std::string toString() const = 0;
#endif

    void writeHeader(BinaryStream& stream) const;

    [[nodiscard]] Result<> readHeader(ReadOnlyBinaryStream& stream);

    void writeWithHeader(BinaryStream& stream) const;

    [[nodiscard]] Result<> readWithHeader(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::inline abi_v975

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#define SCULK_PROTOCOL_PACKET_TO_STRING() [[nodiscard]] std::string toString() const override
#define SCULK_PROTOCOL_PACKET_FORMATTER_IMPL(PACKET_CLASS)                                                             \
    template <>                                                                                                        \
    struct std::formatter<sculk::protocol::abi_v975::PACKET_CLASS> : std::formatter<std::string> {                     \
        auto format(const sculk::protocol::abi_v975::PACKET_CLASS& packet, format_context& ctx) const {                \
            return std::formatter<std::string>::format(packet.toString(), ctx);                                        \
        }                                                                                                              \
    }
#ifdef SCULK_PROTOCOL_HAS_FMTLIB
#define SCULK_PROTOCOL_PACKET_FMT_LIB_FORMATTER_IMPL(PACKET_CLASS)                                                     \
    template <>                                                                                                        \
    struct fmt::formatter<sculk::protocol::abi_v975::PACKET_CLASS> : fmt::formatter<std::string> {                     \
        auto format(const sculk::protocol::abi_v975::PACKET_CLASS& packet, fmt::format_context& ctx) const {           \
            return fmt::formatter<std::string>::format(packet.toString(), ctx);                                        \
        }                                                                                                              \
    }
#define SCULK_PROTOCOL_PACKET_FORMATTER(PACKET_CLASS)                                                                  \
    SCULK_PROTOCOL_PACKET_FORMATTER_IMPL(PACKET_CLASS);                                                                \
    SCULK_PROTOCOL_PACKET_FMT_LIB_FORMATTER_IMPL(PACKET_CLASS);
#else
#define SCULK_PROTOCOL_PACKET_FORMATTER(PACKET_CLASS) SCULK_PROTOCOL_PACKET_FORMATTER_IMPL(PACKET_CLASS);
#endif
#else
#define SCULK_PROTOCOL_PACKET_TO_STRING()
#define SCULK_PROTOCOL_PACKET_FORMATTER(PACKET_CLASS)
#endif

SCULK_PROTOCOL_PACKET_FORMATTER(IPacket)