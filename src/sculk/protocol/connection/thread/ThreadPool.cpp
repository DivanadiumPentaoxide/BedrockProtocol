// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/thread/ThreadPool.hpp"
#include <algorithm>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace thread {

ThreadPool::ThreadPool(std::size_t threadCount) {
    if (threadCount == 0) {
        threadCount = std::max<std::size_t>(1, std::thread::hardware_concurrency());
    }

    mWorkerStates.reserve(threadCount);
    mWorkers.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i) {
        mWorkerStates.emplace_back(std::make_unique<WorkerState>());
        mWorkers.emplace_back([this, i](std::stop_token token) { workerLoop(token, i); });
    }
}

ThreadPool::~ThreadPool() { stop(); }

void ThreadPool::stop() noexcept {
    if (!mAcceptingSubmissions.exchange(false, std::memory_order_acq_rel)) {
        return;
    }

    auto inFlight = mInFlightSubmissions.load(std::memory_order_acquire);
    while (inFlight != 0) {
        mInFlightSubmissions.wait(inFlight, std::memory_order_acquire);
        inFlight = mInFlightSubmissions.load(std::memory_order_acquire);
    }

    if (mStopping.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    for (auto& state : mWorkerStates) {
        state->mSignal.release();
    }

    for (auto& worker : mWorkers) {
        worker.request_stop();
    }
}

void ThreadPool::workerLoop(std::stop_token stopToken, std::size_t workerIndex) {
    auto& state = *mWorkerStates[workerIndex];

    for (;;) {
        state.mSignal.acquire();

        if (mStopping.load(std::memory_order_acquire) || stopToken.stop_requested()) {
            Task task;
            while (state.mTasks.try_dequeue(task)) {
                task();
            }
            return;
        }

        Task task;
        while (state.mTasks.try_dequeue(task)) {
            task();
        }
    }
}

} // namespace thread

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
