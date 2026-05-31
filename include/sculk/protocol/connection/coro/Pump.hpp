// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, you can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "Scheduler.hpp"
#include "Task.hpp"
#include "sculk/protocol/utility/Result.hpp"
#include <concepts>
#include <coroutine>
#include <cstddef>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <vector>

namespace sculk::protocol::inline abi_v975::coro {

class DetachedTask final {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        [[nodiscard]] DetachedTask get_return_object() noexcept {
            return DetachedTask{handle_type::from_promise(*this)};
        }

        [[nodiscard]] std::suspend_always initial_suspend() const noexcept { return {}; }

        struct FinalAwaiter {
            [[nodiscard]] bool await_ready() const noexcept { return false; }

            void await_suspend(handle_type handle) const noexcept {
                if (handle) {
                    handle.destroy();
                }
            }

            void await_resume() const noexcept {}
        };

        [[nodiscard]] FinalAwaiter final_suspend() const noexcept { return {}; }

        void return_void() const noexcept {}

        void unhandled_exception() const noexcept {}
    };

    DetachedTask() = default;

    explicit DetachedTask(handle_type handle) noexcept : mHandle(handle) {}

    DetachedTask(const DetachedTask&)            = delete;
    DetachedTask& operator=(const DetachedTask&) = delete;

    DetachedTask(DetachedTask&& other) noexcept : mHandle(std::exchange(other.mHandle, {})) {}

    DetachedTask& operator=(DetachedTask&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        if (mHandle) {
            mHandle.destroy();
        }

        mHandle = std::exchange(other.mHandle, {});
        return *this;
    }

    ~DetachedTask() {
        if (mHandle) {
            mHandle.destroy();
        }
    }

    [[nodiscard]] handle_type release() noexcept { return std::exchange(mHandle, {}); }

private:
    handle_type mHandle{};
};

inline void startDetached(coro::Scheduler& scheduler, DetachedTask task) {
    auto handle = task.release();
    if (!handle) {
        return;
    }

    if (!scheduler.schedule(handle)) {
        handle.destroy();
    }
}

template <typename Source, typename Sink, typename OnStop = std::nullptr_t>
    requires std::invocable<Source&> && std::is_nothrow_invocable_v<Source&>
          && std::same_as<std::invoke_result_t<Source&>, coro::Task<Result<std::vector<std::byte>>>>
          && std::is_nothrow_move_constructible_v<Source> && std::invocable<Sink&, std::vector<std::byte>&&>
          && std::is_nothrow_invocable_v<Sink&, std::vector<std::byte>&&> && std::is_nothrow_move_constructible_v<Sink>
          && (std::same_as<std::invoke_result_t<Sink&, std::vector<std::byte> &&>, void>
              || std::same_as<std::invoke_result_t<Sink&, std::vector<std::byte> &&>, bool>)
          && (std::same_as<OnStop, std::nullptr_t>
              || (std::invocable<OnStop&> && std::is_nothrow_invocable_v<OnStop&>
                  && std::is_nothrow_move_constructible_v<OnStop>))
DetachedTask packetPump(Source source, Sink sink, OnStop onStop = nullptr) {
    for (;;) {
        auto packet = co_await source();
        if (!packet) {
            if constexpr (!std::is_same_v<OnStop, std::nullptr_t>) {
                onStop();
            }
            co_return;
        }

        using SinkResult = std::invoke_result_t<Sink&, std::vector<std::byte>&&>;
        if constexpr (std::is_void_v<SinkResult>) {
            sink(std::move(*packet));
        } else {
            if (!sink(std::move(*packet))) {
                if constexpr (!std::is_same_v<OnStop, std::nullptr_t>) {
                    onStop();
                }
                co_return;
            }
        }
    }
}

template <typename Source, typename Sink, typename OnStop = std::nullptr_t>
    requires std::invocable<Source&> && std::is_nothrow_invocable_v<Source&>
          && std::same_as<std::invoke_result_t<Source&>, coro::Task<Result<std::vector<std::byte>>>>
          && std::is_nothrow_move_constructible_v<Source> && std::invocable<Sink&, std::vector<std::byte>&&>
          && std::is_nothrow_invocable_v<Sink&, std::vector<std::byte>&&> && std::is_nothrow_move_constructible_v<Sink>
          && (std::same_as<std::invoke_result_t<Sink&, std::vector<std::byte> &&>, void>
              || std::same_as<std::invoke_result_t<Sink&, std::vector<std::byte> &&>, bool>)
          && (std::same_as<OnStop, std::nullptr_t>
              || (std::invocable<OnStop&> && std::is_nothrow_invocable_v<OnStop&>
                  && std::is_nothrow_move_constructible_v<OnStop>))
inline void startPacketPump(coro::Scheduler& scheduler, Source source, Sink sink, OnStop onStop = nullptr) {
    startDetached(scheduler, packetPump(std::move(source), std::move(sink), std::move(onStop)));
}

} // namespace sculk::protocol::inline abi_v975::coro
