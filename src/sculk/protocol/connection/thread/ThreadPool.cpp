// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/thread/ThreadPool.hpp"
#include <algorithm>

namespace sculk::protocol::inline abi_v975 {

namespace thread {

ThreadPool::ThreadPool(std::size_t threadCount) {
    if (threadCount == 0) {
        threadCount = std::max<std::size_t>(1, std::thread::hardware_concurrency());
    }

    mWorkers.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i) {
        mWorkers.emplace_back([this](std::stop_token token) { workerLoop(token); });
    }
}

ThreadPool::~ThreadPool() { stop(); }

void ThreadPool::stop() noexcept {
    if (mStopping.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    const auto wakeCount = mWorkers.size();
    for (std::size_t i = 0; i < wakeCount; ++i) {
        mWorkSignal.release();
    }

    for (auto& worker : mWorkers) {
        worker.request_stop();
    }
}

void ThreadPool::workerLoop(std::stop_token stopToken) {
    for (;;) {
        mWorkSignal.acquire();

        if (mStopping.load(std::memory_order_acquire) || stopToken.stop_requested()) {
            std::function<void()> task;
            while (mTasks.try_dequeue(task)) {
                task();
            }
            return;
        }

        std::function<void()> task;
        while (mTasks.try_dequeue(task)) {
            task();
        }
    }
}

} // namespace thread

} // namespace sculk::protocol::inline abi_v975
