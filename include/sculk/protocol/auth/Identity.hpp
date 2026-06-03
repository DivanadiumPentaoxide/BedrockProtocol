// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/Result.hpp"
#include <cstdint>
#include <string>
#include <string_view>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct Identity {
    std::uint64_t mHighBits{};
    std::uint64_t mLowBits{};

    [[nodiscard]] std::string toString() const;

    [[nodiscard]] static Result<> validateXuid(std::string_view xuid);

    [[nodiscard]] static Identity fromXuid(std::string_view xuid) noexcept;

    [[nodiscard]] static Result<> validateString(std::string_view str);

    [[nodiscard]] static Identity fromString(std::string_view str) noexcept;
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE