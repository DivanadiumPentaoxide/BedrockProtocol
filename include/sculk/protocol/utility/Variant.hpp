// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "Result.hpp"
#include <type_traits>
#include <utility>
#include <variant>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace detail {

template <std::size_t I, typename Var>
constexpr Result<> emplace_variant_switch(Var& v, std::size_t idx _SCULK_SL_PARAMETER_DEF) noexcept {
    constexpr std::size_t N = std::variant_size_v<std::remove_reference_t<Var>>;

    if (idx == I) {
        v.template emplace<I>();
        return {};
    }

    if constexpr (I + 1 < N) {
        return emplace_variant_switch<I + 1>(v, idx _SCULK_SL_PARAM_PASS);
    }

#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
    return error_utils::makeError(
        std::format("failed to emplace variant: index {} out of range, max index is {}", idx, N - 1),
        location
    );
#else
    return error_utils::makeError("failed to emplace variant: index out of range");
#endif
}

} // namespace detail

template <typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
Overload(Ts...) -> Overload<Ts...>;

template <typename Var>
constexpr Result<> emplace_variant(Var& v, std::size_t idx _SCULK_SL_PARAMETER_DEF) noexcept {
    return detail::emplace_variant_switch<0>(v, idx _SCULK_SL_PARAM_PASS);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
