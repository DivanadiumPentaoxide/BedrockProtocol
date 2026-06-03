// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <semaphore>
#include <thread>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class ClientNetworkSystem;

namespace io {

class ClientIoRuntime final {
public:
    explicit ClientIoRuntime(std::size_t workerCount = 1);

    ClientIoRuntime(const ClientIoRuntime&)            = delete;
    ClientIoRuntime& operator=(const ClientIoRuntime&) = delete;
    ClientIoRuntime(ClientIoRuntime&&)                 = delete;
    ClientIoRuntime& operator=(ClientIoRuntime&&)      = delete;

    ~ClientIoRuntime();

public:
    void registerClient(ClientNetworkSystem& client);

    void unregisterClient(ClientNetworkSystem& client) noexcept;

    void notifyWork() noexcept;

private:
    struct Registration;
    using RegistrationList = std::vector<std::shared_ptr<Registration>>;

    void workerLoop(std::stop_token stopToken, std::size_t workerIndex);

private:
    std::size_t                                          mWorkerCount{};
    std::atomic<std::shared_ptr<const RegistrationList>> mClientsSnapshot{};
    std::counting_semaphore<>                            mWakeSignal{0};
    std::atomic_bool                                     mWakePending{false};
    std::vector<std::size_t>                             mWorkerNextStartSlot{};
    std::vector<std::jthread>                            mWorkers{};
};

} // namespace io

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
