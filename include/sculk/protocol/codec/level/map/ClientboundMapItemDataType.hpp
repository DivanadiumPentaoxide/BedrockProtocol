// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <cstdint>
#include <utility>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class ClientboundMapItemDataType : std::uint32_t {
    Invalid          = 0,
    TextureUpdate    = 1 << 1,
    DecorationUpdate = 1 << 2,
    Creation         = 1 << 3,
    All              = TextureUpdate | DecorationUpdate | Creation,
};

[[nodiscard]] constexpr ClientboundMapItemDataType
operator|(const ClientboundMapItemDataType lhs, const ClientboundMapItemDataType rhs) noexcept {
    return static_cast<ClientboundMapItemDataType>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr ClientboundMapItemDataType
operator&(const ClientboundMapItemDataType lhs, const ClientboundMapItemDataType rhs) noexcept {
    return static_cast<ClientboundMapItemDataType>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
