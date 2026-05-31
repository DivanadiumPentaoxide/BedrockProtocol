// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/coro/Scheduler.hpp"

namespace sculk::protocol::inline abi_v975 {

namespace coro {

bool Scheduler::schedule(std::coroutine_handle<> handle) noexcept {
    return mPool.submit([handle]() noexcept {
        if (handle) {
            handle.resume();
        }
    });
}

} // namespace coro

} // namespace sculk::protocol::inline abi_v975
