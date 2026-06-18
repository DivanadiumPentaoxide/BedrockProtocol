// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <atomic>
#include <memory>
#include <utility>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

template <typename T>
class AtomicSharedPtr final {
public:
    AtomicSharedPtr() noexcept = default;

    explicit AtomicSharedPtr(std::shared_ptr<T> value) noexcept : mValue(std::move(value)) {}

    AtomicSharedPtr(const AtomicSharedPtr&)            = delete;
    AtomicSharedPtr& operator=(const AtomicSharedPtr&) = delete;

    [[nodiscard]] std::shared_ptr<T> load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
#ifndef _LIBCPP_VERSION
        return mValue.load(order);
#else
        return std::atomic_load_explicit(&mValue, order);
#endif
    }

    void store(std::shared_ptr<T> value, std::memory_order order = std::memory_order_seq_cst) noexcept {
#ifndef _LIBCPP_VERSION
        mValue.store(std::move(value), order);
#else
        std::atomic_store_explicit(&mValue, std::move(value), order);
#endif
    }

    [[nodiscard]] std::shared_ptr<T>
    exchange(std::shared_ptr<T> value, std::memory_order order = std::memory_order_seq_cst) noexcept {
#ifndef _LIBCPP_VERSION
        return mValue.exchange(std::move(value), order);
#else
        return std::atomic_exchange_explicit(&mValue, std::move(value), order);
#endif
    }

    [[nodiscard]] bool compare_exchange_weak(
        std::shared_ptr<T>& expected,
        std::shared_ptr<T>  desired,
        std::memory_order   success,
        std::memory_order   failure
    ) noexcept {
#ifndef _LIBCPP_VERSION
        return mValue.compare_exchange_weak(expected, std::move(desired), success, failure);
#else
        return std::atomic_compare_exchange_weak_explicit(&mValue, &expected, std::move(desired), success, failure);
#endif
    }

private:
#ifndef _LIBCPP_VERSION
    std::atomic<std::shared_ptr<T>> mValue{};
#else
    std::shared_ptr<T> mValue{};
#endif
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE