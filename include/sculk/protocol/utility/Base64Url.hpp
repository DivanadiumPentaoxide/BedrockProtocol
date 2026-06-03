// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/Result.hpp"
#include <cstdint>
#include <span>
#include <string>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::base64url {

namespace detail {

constexpr std::int8_t encodeLookup(std::int8_t c) noexcept {
    return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"[c];
}

constexpr std::int8_t decodeLookup(std::int8_t c) noexcept {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 71;
    if (c >= '0' && c <= '9') return c + 4;
    if (c == '-') return 62;
    if (c == '_') return 63;
    return 64;
}

constexpr bool isEncodedChar(char c) noexcept {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_'
        || c == '=';
}

constexpr bool hasValidPadding(std::string_view in) noexcept {
    std::size_t firstPadding = in.find('=');
    if (firstPadding == std::string_view::npos) {
        return true;
    }

    for (std::size_t i = firstPadding; i < in.size(); ++i) {
        if (in[i] != '=') {
            return false;
        }
    }

    return in.size() - firstPadding <= 2;
}

} // namespace detail

constexpr std::size_t getEncodeLength(std::size_t len) noexcept { return (len + 2 - ((len + 2) % 3)) / 3 * 4; }

constexpr std::size_t getEncodeLength(std::string_view str) noexcept { return getEncodeLength(str.length()); }

constexpr std::size_t getDecodeLength(std::string_view in) noexcept {
    std::uint8_t count      = 0;
    std::size_t  input_size = in.size();
    for (auto it = in.rbegin(); it != in.rend() && *it == '='; ++it) {
        ++count;
    }
    input_size -= count;
    count       = 0;
    while (input_size % 4) {
        input_size++;
        count++;
    }
    return ((6 * input_size) / 8) - count;
}

constexpr std::string encode(std::string_view str) noexcept {
    std::string result;
    result.reserve(getEncodeLength(str));
    std::int32_t i = 0;
    std::int32_t j = -6;
    for (auto& c : str) {
        i  = (i << 8) + static_cast<std::uint8_t>(c);
        j += 8;
        while (j >= 0) {
            result += detail::encodeLookup((i >> j) & 0x3F);
            j      -= 6;
        }
    }
    if (j > -6) {
        result += detail::encodeLookup(((i << 8) >> (j + 8)) & 0x3F);
    }
    return result;
}

[[nodiscard]] inline std::string encode(std::span<const std::uint8_t> bytes) noexcept {
    if (bytes.empty()) {
        return {};
    }
    return encode(std::string_view(reinterpret_cast<const char*>(bytes.data()), bytes.size()));
}

constexpr std::string decode(std::string_view str) noexcept {
    const std::size_t input_size = str.size();
    std::string       out;

    const std::size_t output_size = getDecodeLength(str);

    out.resize(output_size);
    auto next_char = [&](std::size_t& pos) -> std::uint32_t {
        if (pos >= input_size) return 0;

        const char ch = str[pos++];
        if (ch == '=') {
            return 0;
        }
        return static_cast<std::uint32_t>(detail::decodeLookup(ch));
    };

    std::size_t pos = 0;
    std::size_t j   = 0;

    while (pos < input_size) {
        std::uint32_t c1 = next_char(pos);
        std::uint32_t c2 = next_char(pos);
        std::uint32_t c3 = next_char(pos);
        std::uint32_t c4 = next_char(pos);

        const std::uint32_t data = (c1 << 18) | (c2 << 12) | (c3 << 6) | c4;

        if (j < output_size) out[j++] = static_cast<char>((data >> 16) & 0xFF);
        if (j < output_size) out[j++] = static_cast<char>((data >> 8) & 0xFF);
        if (j < output_size) out[j++] = static_cast<char>(data & 0xFF);
    }

    if (j < output_size) {
        out.resize(j);
    }

    return out;
}

[[nodiscard]] inline Result<std::string> decodeChecked(std::string_view str) {
    if (str.empty()) {
        return error_utils::makeError("Base64Url input is empty");
    }

    if (!detail::hasValidPadding(str)) {
        return error_utils::makeError("Base64Url input has invalid padding");
    }

    if (str.size() % 4 == 1) {
        return error_utils::makeError("Base64Url input length is invalid");
    }

    for (char c : str) {
        if (!detail::isEncodedChar(c)) {
            return error_utils::makeError("Base64Url input contains invalid characters");
        }
    }

    return decode(str);
}

[[nodiscard]] inline Result<std::string> decodeChecked(std::string_view str, std::size_t expectedDecodedLength) {
    if (str.empty()) {
        return error_utils::makeError("Base64Url input is empty");
    }

    if (!detail::hasValidPadding(str)) {
        return error_utils::makeError("Base64Url input has invalid padding");
    }

    if (str.size() % 4 == 1) {
        return error_utils::makeError("Base64Url input length is invalid");
    }

    if (getDecodeLength(str) != expectedDecodedLength) {
        return error_utils::makeError("Base64Url decoded length does not match expected length");
    }

    for (char c : str) {
        if (!detail::isEncodedChar(c)) {
            return error_utils::makeError("Base64Url input contains invalid characters");
        }
    }

    return decode(str);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::base64url
