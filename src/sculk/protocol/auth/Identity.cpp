// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/auth/Identity.hpp"
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <openssl/evp.h>

namespace sculk::protocol::inline abi_v975 {

namespace {

constexpr std::string_view     IDENTITY_XUID_PREFIX = "pocket-auth-1-xuid:";
constexpr std::array<char, 16> HEX_DIGITS =
    {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

using MdCtxPtr = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

[[nodiscard]] inline bool
md5(std::string_view inputA, std::string_view inputB, std::array<std::byte, 16>& out) noexcept {
    MdCtxPtr ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    if (!ctx) {
        return false;
    }

    bool success = EVP_DigestInit_ex(ctx.get(), EVP_md5(), nullptr) == 1
                && EVP_DigestUpdate(ctx.get(), inputA.data(), inputA.size()) == 1
                && EVP_DigestUpdate(ctx.get(), inputB.data(), inputB.size()) == 1;

    std::uint32_t digestLen{};
    if (success) {
        success = EVP_DigestFinal_ex(ctx.get(), reinterpret_cast<std::uint8_t*>(out.data()), &digestLen) == 1
               && digestLen == out.size();
    }
    return success;
}

[[nodiscard]] constexpr std::uint64_t readU64BigEndian(const std::byte* bytes) noexcept {
    std::uint64_t value{};
    for (std::size_t i = 0; i < 8; ++i) {
        value = (value << 8) | static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(bytes[i]));
    }
    return value;
}

[[nodiscard]] constexpr bool isAsciiHexDigit(char c) noexcept {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

[[nodiscard]] constexpr std::uint8_t hexValue(char c) noexcept {
    if (c >= '0' && c <= '9') {
        return static_cast<std::uint8_t>(c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return static_cast<std::uint8_t>(c - 'a' + 10);
    }
    return static_cast<std::uint8_t>(c - 'A' + 10);
}

[[nodiscard]] bool parseHex(std::string_view value, std::uint64_t& out) noexcept {
    out = 0;
    for (char c : value) {
        if (!isAsciiHexDigit(c)) {
            return false;
        }
        out = (out << 4) | hexValue(c);
    }
    return true;
}

[[nodiscard]] bool
parseUuidSegment(std::string_view str, std::size_t offset, std::size_t length, std::uint64_t& out) noexcept {
    return parseHex(std::string_view(str.data() + offset, length), out);
}

[[nodiscard]] bool parseIdentityString(std::string_view str, Identity& identity) noexcept {
    if (str.size() != 36) {
        return false;
    }
    if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
        return false;
    }

    std::uint64_t group1{};
    std::uint64_t group2{};
    std::uint64_t group3{};
    std::uint64_t group4{};
    std::uint64_t group5{};
    if (!parseUuidSegment(str, 0, 8, group1) || !parseUuidSegment(str, 9, 4, group2)
        || !parseUuidSegment(str, 14, 4, group3) || !parseUuidSegment(str, 19, 4, group4)
        || !parseUuidSegment(str, 24, 12, group5) || group2 > 0xFFFFu || group3 > 0xFFFFu || group4 > 0xFFFFu
        || group5 > 0x0000FFFFFFFFFFFFull) {
        return false;
    }

    identity.mHighBits = (group1 << 32) | (group2 << 16) | group3;
    identity.mLowBits  = (group4 << 48) | group5;
    return true;
}

} // namespace

std::string Identity::toString() const {
    std::array<char, 36>               result{};
    const std::array<std::uint64_t, 5> parts = {
        (mHighBits >> 32) & 0xFFFFFFFFull,
        (mHighBits >> 16) & 0xFFFFull,
        mHighBits & 0xFFFFull,
        (mLowBits >> 48) & 0xFFFFull,
        mLowBits & 0x0000FFFFFFFFFFFFull,
    };
    const std::array<std::size_t, 5> widths = {8, 4, 4, 4, 12};

    std::size_t writePos = 0;
    for (std::size_t partIndex = 0; partIndex < 5; ++partIndex) {
        const std::size_t width = widths[partIndex];
        for (std::size_t shift = width * 4; shift > 0; shift -= 4) {
            result[writePos++] = HEX_DIGITS[(parts[partIndex] >> (shift - 4)) & 0x0Fu];
        }
        if (partIndex != 4) {
            result[writePos++] = '-';
        }
    }

    return std::string(result.data(), result.size());
}

Result<> Identity::validateXuid(std::string_view xuid) {
    std::uint64_t xuidValue{};
    auto          result = std::from_chars(xuid.data(), xuid.data() + xuid.size(), xuidValue);
    if (result.ec != std::errc{}) {
        return error_utils::makeError("XUID is not a valid unsigned 64-bit integer");
    }
    if (result.ptr != xuid.data() + xuid.size()) {
        return error_utils::makeError("XUID contains invalid characters");
    }
    return {};
}

Identity Identity::fromXuid(std::string_view xuid) noexcept {
    std::array<std::byte, 16> digest{};

    if (!validateXuid(xuid)) {
        std::abort();
    }

    if (!md5(IDENTITY_XUID_PREFIX, xuid, digest)) {
        std::abort();
    }

    digest[6] = static_cast<std::byte>((std::to_integer<std::uint8_t>(digest[6]) & 0x0F) | 0x30); // Version 3
    digest[8] = static_cast<std::byte>((std::to_integer<std::uint8_t>(digest[8]) & 0x3F) | 0x80); // RFC 4122

    return Identity{.mHighBits = readU64BigEndian(digest.data()), .mLowBits = readU64BigEndian(digest.data() + 8)};
}

Result<> Identity::validateString(std::string_view str) {
    Identity identity{};
    if (parseIdentityString(str, identity)) {
        return {};
    }

    if (str.size() != 36) {
        return error_utils::makeError("Identity string must be 36 characters long");
    }
    if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
        return error_utils::makeError("Identity string must be in the format XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX");
    }
    return error_utils::makeError("Identity string contains invalid characters");
}

Identity Identity::fromString(std::string_view str) noexcept {
    Identity identity{};

    if (!parseIdentityString(str, identity)) {
        std::abort();
    }

    return identity;
}

} // namespace sculk::protocol::inline abi_v975
