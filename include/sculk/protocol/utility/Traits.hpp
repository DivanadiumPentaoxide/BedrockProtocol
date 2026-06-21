// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <tuple>
#include <type_traits>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::traits {

template <typename MemberFuncPtr>
struct MemberFuncPtrTraitsImpl;

template <typename Class, typename Ret, typename... Args>
struct MemberFuncPtrTraitsImpl<Ret (Class::*)(Args...)> {
    using class_type  = Class;
    using return_type = Ret;
    using args_tuple  = std::tuple<Args...>;

    template <std::size_t N>
    using arg_type = std::tuple_element_t<N, args_tuple>;
};

template <typename Class, typename Ret, typename... Args>
struct MemberFuncPtrTraitsImpl<Ret (Class::*)(Args...) noexcept> : MemberFuncPtrTraitsImpl<Ret (Class::*)(Args...)> {};

template <typename MemberFuncPtr>
struct MemberFuncPtrTraits : MemberFuncPtrTraitsImpl<std::remove_cv_t<std::remove_reference_t<MemberFuncPtr>>> {};

template <typename MemberFuncPtr>
using member_func_class_t = typename MemberFuncPtrTraits<MemberFuncPtr>::class_type;

template <typename MemberFuncPtr>
using member_func_return_t = typename MemberFuncPtrTraits<MemberFuncPtr>::return_type;

template <typename MemberFuncPtr, std::size_t N>
using member_func_arg_t = typename MemberFuncPtrTraits<MemberFuncPtr>::template arg_type<N>;

template <typename T>
constexpr bool always_false_v = false;

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::traits