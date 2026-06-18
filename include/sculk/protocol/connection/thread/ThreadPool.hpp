// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <atomic>
#include <concepts>
#include <concurrentqueue.h>
#include <cstddef>
#include <functional>
#include <memory>
#include <semaphore>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

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
        requires std::invocable<F&> && std::is_nothrow_invocable_v<F&>
    bool submit(F&& task) {
        if (!mAcceptingSubmissions.load(std::memory_order_acquire)) {
            return false;
        }

        mInFlightSubmissions.fetch_add(1, std::memory_order_acq_rel);

        if (!mAcceptingSubmissions.load(std::memory_order_acquire)) {
            if (mInFlightSubmissions.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                mInFlightSubmissions.notify_all();
            }
            return false;
        }

        const auto workerCount = mWorkerStates.size();
        if (workerCount == 0) {
            if (mInFlightSubmissions.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                mInFlightSubmissions.notify_all();
            }
            return false;
        }

        const auto workerIndex = mNextWorker.fetch_add(1, std::memory_order_relaxed) % workerCount;
        auto&      workerState = *mWorkerStates[workerIndex];

        auto       wrapped  = Task(std::forward<F>(task));
        const bool enqueued = workerState.mTasks.enqueue(std::move(wrapped));

        if (mInFlightSubmissions.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            mInFlightSubmissions.notify_all();
        }

        if (!enqueued) {
            return false;
        }

        workerState.mSignal.release();
        return true;
    }

    [[nodiscard]] std::size_t threadCount() const noexcept { return mWorkers.size(); }

    [[nodiscard]] bool isStopping() const noexcept { return mStopping.load(std::memory_order_acquire); }

    void stop() noexcept;

private:
#ifndef _LIBCPP_VERSION
    using Task = std::move_only_function<void() noexcept>;
#else
    class Task final {
    private:
        struct Concept {
            virtual ~Concept()           = default;
            virtual void call() noexcept = 0;
        };

        template <typename F>
        struct Model final : Concept {
            F mFunction;

            explicit Model(F&& function) noexcept(std::is_nothrow_move_constructible_v<F>)
            : mFunction(std::move(function)) {}

            void call() noexcept override { std::invoke(mFunction); }
        };

    public:
        Task() = default;

        template <typename F>
            requires std::invocable<F&>
        explicit Task(F&& function) : mFunction(std::make_unique<Model<std::decay_t<F>>>(std::forward<F>(function))) {}

        Task(const Task&)            = delete;
        Task& operator=(const Task&) = delete;

        Task(Task&&) noexcept            = default;
        Task& operator=(Task&&) noexcept = default;

        void operator()() noexcept { mFunction->call(); }

    private:
        std::unique_ptr<Concept> mFunction{};
    };
#endif

    struct WorkerState final {
        moodycamel::ConcurrentQueue<Task>    mTasks{};
        std::counting_semaphore<1024 * 1024> mSignal{0};
    };

private:
    void workerLoop(std::stop_token stopToken, std::size_t workerIndex);

private:
    std::atomic<std::size_t>                  mNextWorker{0};
    std::atomic_bool                          mAcceptingSubmissions{true};
    std::atomic_uint32_t                      mInFlightSubmissions{0};
    std::atomic_bool                          mStopping{false};
    std::vector<std::unique_ptr<WorkerState>> mWorkerStates{};
    std::vector<std::jthread>                 mWorkers{};
};

} // namespace thread

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
