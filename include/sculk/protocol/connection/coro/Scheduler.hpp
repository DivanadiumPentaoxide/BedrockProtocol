// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/connection/thread/ThreadPool.hpp"
#include <coroutine>

namespace sculk::protocol::inline abi_v975 {

namespace coro {

class Scheduler final {
public:
    explicit Scheduler(thread::ThreadPool& pool) noexcept : mPool(pool) {}

    Scheduler(const Scheduler&)            = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&)                 = delete;
    Scheduler& operator=(Scheduler&&)      = delete;

public:
    bool schedule(std::coroutine_handle<> handle) noexcept;

    template <typename F>
    bool schedule(F&& func) {
        return mPool.submit(std::forward<F>(func));
    }

private:
    thread::ThreadPool& mPool;
};

} // namespace coro

} // namespace sculk::protocol::inline abi_v975
