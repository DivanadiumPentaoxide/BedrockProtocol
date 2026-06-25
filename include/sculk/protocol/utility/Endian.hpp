// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::endian {

template <typename>
inline constexpr bool dependent_false_v = false;

template <typename T>
    requires std::is_trivially_copyable_v<T>
[[nodiscard]] constexpr T swapEndian(T value) noexcept {
    if constexpr (sizeof(T) == 1) {
        return value;
    } else if constexpr (std::is_integral_v<T>) {
        return std::byteswap(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
            return std::bit_cast<T>(std::byteswap(std::bit_cast<std::uint32_t>(value)));
        } else if constexpr (sizeof(T) == sizeof(std::uint64_t)) {
            return std::bit_cast<T>(std::byteswap(std::bit_cast<std::uint64_t>(value)));
        } else {
            static_assert(dependent_false_v<T>, "unknown floating point size for endian swap");
        }
    } else {
        auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
        std::reverse(bytes.begin(), bytes.end());
        return std::bit_cast<T>(bytes);
    }
}

template <typename T>
    requires std::is_trivially_copyable_v<T>
[[nodiscard]] constexpr T toLittleEndian(T value) noexcept {
    if constexpr (std::endian::native == std::endian::little) {
        return value;
    } else {
        return swapEndian(value);
    }
}

template <typename T>
    requires std::is_trivially_copyable_v<T>
[[nodiscard]] constexpr T fromLittleEndian(T value) noexcept {
    if constexpr (std::endian::native == std::endian::little) {
        return value;
    } else {
        return swapEndian(value);
    }
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::endian