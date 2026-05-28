// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "BatchedNetworkPeer.hpp"

namespace sculk::protocol::inline abi_v975 {

class ISession {
    BatchedNetworkPeer mPeer{};

public:
    virtual ~ISession() = default;

public:
    [[nodiscard]] constexpr BatchedNetworkPeer& getPeer() { return mPeer; }

    [[nodiscard]] constexpr const BatchedNetworkPeer& getPeer() const { return mPeer; }
};

} // namespace sculk::protocol::inline abi_v975