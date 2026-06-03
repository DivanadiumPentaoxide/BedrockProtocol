// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/io/ClientIoRuntime.hpp"
#include "sculk/protocol/connection/ClientNetworkSystem.hpp"
#include <algorithm>
#include <chrono>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::io {

namespace {

constexpr auto        IO_RUNTIME_IDLE_WAIT            = std::chrono::milliseconds(1);
constexpr auto        IO_RUNTIME_MAX_TICK_BUDGET      = std::chrono::microseconds(1500);
constexpr std::size_t IO_RUNTIME_MAX_CLIENTS_PER_PASS = 64;

} // namespace

struct ClientIoRuntime::Registration {
    ClientNetworkSystem* mClient{};
    std::atomic_bool     mClosed{false};
    std::atomic_uint32_t mActiveTicks{0};

    [[nodiscard]] bool tryEnter() {
        if (mClosed.load(std::memory_order_acquire)) {
            return false;
        }

        mActiveTicks.fetch_add(1, std::memory_order_acq_rel);
        if (mClosed.load(std::memory_order_acquire)) {
            leave();
            return false;
        }

        return true;
    }

    void leave() {
        if (mActiveTicks.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            mActiveTicks.notify_all();
        }
    }

    void closeAndWait() {
        mClosed.store(true, std::memory_order_release);

        auto activeTicks = mActiveTicks.load(std::memory_order_acquire);
        while (activeTicks != 0) {
            mActiveTicks.wait(activeTicks, std::memory_order_acquire);
            activeTicks = mActiveTicks.load(std::memory_order_acquire);
        }
    }
};

ClientIoRuntime::ClientIoRuntime(std::size_t workerCount)
: mWorkerCount(std::max<std::size_t>(std::size_t{1}, workerCount)),
  mClientsSnapshot(std::shared_ptr<const RegistrationList>{std::make_shared<RegistrationList>()}),
  mWorkerNextStartSlot(mWorkerCount, 0) {
    mWorkers.reserve(mWorkerCount);
    for (std::size_t i = 0; i < mWorkerCount; ++i) {
        mWorkers.emplace_back([this, i](std::stop_token stopToken) { workerLoop(stopToken, i); });
    }
}

ClientIoRuntime::~ClientIoRuntime() {
    for (auto& worker : mWorkers) {
        worker.request_stop();
    }

    if (mWorkerCount > 0) {
        mWakeSignal.release(static_cast<std::ptrdiff_t>(mWorkerCount));
    }
}

void ClientIoRuntime::registerClient(ClientNetworkSystem& client) {
    auto registration     = std::make_shared<Registration>();
    registration->mClient = &client;

    for (;;) {
        auto current = mClientsSnapshot.load(std::memory_order_acquire);
        auto next    = std::make_shared<RegistrationList>(*current);
        auto exists  = std::ranges::find_if(*next, [&client](const auto& currentRegistration) {
            return currentRegistration && currentRegistration->mClient == &client;
        });
        if (exists != next->end()) {
            break;
        }

        next->push_back(registration);
        std::shared_ptr<const RegistrationList> desired = next;
        if (mClientsSnapshot
                .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
            break;
        }
    }

    notifyWork();
}

void ClientIoRuntime::unregisterClient(ClientNetworkSystem& client) noexcept {
    std::shared_ptr<Registration> registration;
    for (;;) {
        auto current = mClientsSnapshot.load(std::memory_order_acquire);
        auto next    = std::make_shared<RegistrationList>();
        next->reserve(current->size());

        for (const auto& currentRegistration : *current) {
            if (!registration && currentRegistration && currentRegistration->mClient == &client) {
                registration = currentRegistration;
                continue;
            }

            next->push_back(currentRegistration);
        }

        if (!registration) {
            return;
        }

        std::shared_ptr<const RegistrationList> desired = next;
        if (mClientsSnapshot
                .compare_exchange_weak(current, desired, std::memory_order_release, std::memory_order_acquire)) {
            break;
        }

        registration.reset();
    }

    registration->closeAndWait();
    notifyWork();
}

void ClientIoRuntime::notifyWork() noexcept {
    if (!mWakePending.exchange(true, std::memory_order_acq_rel)) {
        mWakeSignal.release(static_cast<std::ptrdiff_t>(mWorkerCount));
    }
}

void ClientIoRuntime::workerLoop(std::stop_token stopToken, std::size_t workerIndex) {
    while (!stopToken.stop_requested()) {
        mWakePending.store(false, std::memory_order_release);

        auto snapshot   = mClientsSnapshot.load(std::memory_order_acquire);
        bool progressed = false;

        if (snapshot->size() <= workerIndex) {
            (void)mWakeSignal.try_acquire_for(IO_RUNTIME_IDLE_WAIT);
            continue;
        }

        const auto assignedSlots = 1 + ((snapshot->size() - 1 - workerIndex) / mWorkerCount);
        const auto startSlot     = mWorkerNextStartSlot[workerIndex] % assignedSlots;
        const auto tickDeadline  = std::chrono::steady_clock::now() + IO_RUNTIME_MAX_TICK_BUDGET;

        std::size_t scannedSlots  = 0;
        std::size_t tickedClients = 0;
        bool        budgetStopped = false;
        for (; scannedSlots < assignedSlots; ++scannedSlots) {
            if (tickedClients >= IO_RUNTIME_MAX_CLIENTS_PER_PASS) {
                budgetStopped = true;
                break;
            }

            if (std::chrono::steady_clock::now() >= tickDeadline) {
                budgetStopped = true;
                break;
            }

            const auto  slotIndex    = (startSlot + scannedSlots) % assignedSlots;
            const auto  clientIndex  = workerIndex + (slotIndex * mWorkerCount);
            const auto& registration = (*snapshot)[clientIndex];
            if (!registration || !registration->tryEnter()) {
                continue;
            }

            ++tickedClients;
            progressed = registration->mClient->ioTickOnce() || progressed;
            registration->leave();
        }

        mWorkerNextStartSlot[workerIndex] = (startSlot + scannedSlots) % assignedSlots;

        if (progressed || budgetStopped) {
            continue;
        }

        (void)mWakeSignal.try_acquire_for(IO_RUNTIME_IDLE_WAIT);
    }
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::io
