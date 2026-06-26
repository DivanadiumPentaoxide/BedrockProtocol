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
constexpr Result<> emplace_variant_switch(Var& v, std::size_t index _SCULK_SL_PARAMETER_DEF) noexcept {
    constexpr std::size_t N = std::variant_size_v<std::remove_reference_t<Var>>;

    if (index == I) {
        v.template emplace<I>();
        return {};
    }

    if constexpr (I + 1 < N) {
        return emplace_variant_switch<I + 1>(v, index _SCULK_SL_PARAM_PASS);
    }

#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
    return error_utils::makeError(
        std::format("failed to emplace variant: index {} out of range, max index is {}", index, N - 1),
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
constexpr Result<> emplace_variant(Var& v, std::size_t index _SCULK_SL_PARAMETER_DEF) noexcept {
    return detail::emplace_variant_switch<0>(v, index _SCULK_SL_PARAM_PASS);
}

template <typename T, typename... Type>
    requires std::is_enum_v<T>
struct TaggedVariant {
    std::variant<Type...> mValue;

    using tag_enum_type = T;

    template <typename... Args>
    constexpr TaggedVariant(Args&&... args) : mValue(std::forward<Args>(args)...) {}

    template <typename V>
    constexpr auto visit(V&& visitor) {
        return std::visit(std::forward<V>(visitor), mValue);
    }

    template <typename V>
    constexpr auto visit(V&& visitor) const {
        return std::visit(std::forward<V>(visitor), mValue);
    }

    constexpr T type() const noexcept { return static_cast<T>(mValue.index()); }

    constexpr Result<> set(T typeIndex _SCULK_SL_PARAMETER_DEF) noexcept {
        return emplace_variant(mValue, static_cast<std::size_t>(typeIndex) _SCULK_SL_PARAM_PASS);
    }

    template <typename... Args>
    auto emplace() {
        return mValue.template emplace<Args...>();
    }
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
