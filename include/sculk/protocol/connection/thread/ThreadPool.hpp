// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <atomic>
#include <concepts>
#include <concurrentqueue.h>
#include <cstddef>
#include <functional>
#include <memory>
#include <semaphore>
#include <thread>
#include <vector>

namespace sculk::protocol::inline abi_v975 {

namespace thread {

class ThreadPool final {
public:
    explicit ThreadPool(std::size_t threadCount = 0);

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&)                 = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    ~ThreadPool();

public:
    template <typename F>
        requires std::invocable<F>
    bool submit(F&& task) {
        if (mStopping.load(std::memory_order_acquire)) {
            return false;
        }

        auto wrapped = std::function<void()>(std::forward<F>(task));
        if (!mTasks.enqueue(std::move(wrapped))) {
            return false;
        }
        mWorkSignal.release();
        return true;
    }

    [[nodiscard]] std::size_t threadCount() const noexcept { return mWorkers.size(); }

    [[nodiscard]] bool isStopping() const noexcept { return mStopping.load(std::memory_order_acquire); }

    void stop() noexcept;

private:
    void workerLoop(std::stop_token stopToken);

private:
    moodycamel::ConcurrentQueue<std::function<void()>> mTasks{};
    std::counting_semaphore<1024 * 1024>               mWorkSignal{0};
    std::atomic_bool                                   mStopping{false};
    std::vector<std::jthread>                          mWorkers{};
};

} // namespace thread

} // namespace sculk::protocol::inline abi_v975
