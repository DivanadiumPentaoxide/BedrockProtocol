// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "Scheduler.hpp"
#include "sculk/protocol/utility/Result.hpp"
#include <concepts>
#include <coroutine>
#include <optional>
#include <type_traits>
#include <utility>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace coro {

namespace {

template <typename>
struct is_result_type : std::false_type {};

template <typename T>
struct is_result_type<Result<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_result_type_v = is_result_type<T>::value;

} // namespace

template <typename T>
class [[nodiscard]] Task final {
    static_assert(is_result_type_v<T>, "Task only supports Result<T> payloads");
    static_assert(std::is_nothrow_move_constructible_v<T>, "Task payload must be nothrow-move-constructible");
    static_assert(std::is_nothrow_destructible_v<T>, "Task payload must be nothrow-destructible");

public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        std::optional<T>        mValue{};
        std::coroutine_handle<> mContinuation{};

        [[nodiscard]] Task get_return_object() noexcept { return Task{handle_type::from_promise(*this)}; }

        [[nodiscard]] std::suspend_always initial_suspend() const noexcept { return {}; }

        struct FinalAwaiter {
            [[nodiscard]] bool await_ready() const noexcept { return false; }

            [[nodiscard]] std::coroutine_handle<> await_suspend(handle_type handle) const noexcept {
                if (handle.promise().mContinuation) {
                    return handle.promise().mContinuation;
                }
                return std::noop_coroutine();
            }

            void await_resume() const noexcept {}
        };

        [[nodiscard]] FinalAwaiter final_suspend() const noexcept { return {}; }

        template <typename U>
            requires std::constructible_from<T, U&&>
        void return_value(U&& input) noexcept(noexcept(mValue.emplace(std::forward<U>(input)))) {
            mValue.emplace(std::forward<U>(input));
        }

        void unhandled_exception() noexcept { mValue.emplace(error_utils::makeError("unhandled coroutine exception")); }
    };

public:
    Task() = default;

    explicit Task(handle_type handle) noexcept : mHandle(handle) {}

    Task(const Task&)            = delete;
    Task& operator=(const Task&) = delete;

    Task(Task&& other) noexcept : mHandle(std::exchange(other.mHandle, {})) {}

    Task& operator=(Task&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        if (mHandle) {
            mHandle.destroy();
        }
        mHandle = std::exchange(other.mHandle, {});
        return *this;
    }

    ~Task() noexcept {
        if (mHandle) {
            mHandle.destroy();
        }
    }

public:
    [[nodiscard]] bool valid() const noexcept { return static_cast<bool>(mHandle); }

    [[nodiscard]] bool done() const noexcept { return !mHandle || mHandle.done(); }

    bool start(coro::Scheduler& scheduler) noexcept {
        if (!mHandle) {
            return false;
        }
        auto handle = std::exchange(mHandle, {});
        return scheduler.schedule(handle);
    }

    struct Awaiter {
        handle_type mHandle;

        [[nodiscard]] bool await_ready() const noexcept { return !mHandle || mHandle.done(); }

        [[nodiscard]] std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept {
            mHandle.promise().mContinuation = continuation;
            return mHandle;
        }

        T await_resume() noexcept {
            auto& promise = mHandle.promise();
            if (!promise.mValue.has_value()) {
                return error_utils::makeError("task completed without value");
            }
            return std::move(*promise.mValue);
        }
    };

    [[nodiscard]] Awaiter operator co_await() && noexcept { return Awaiter{mHandle}; }

private:
    handle_type mHandle{};
};

} // namespace coro

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
