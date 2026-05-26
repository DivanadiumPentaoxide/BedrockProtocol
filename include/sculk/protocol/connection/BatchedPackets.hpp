// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include <memory>
#include <vector>

namespace sculk::protocol::inline abi_v975 {

class BatchedPackets {
public:
    std::vector<std::unique_ptr<IPacket>> mPackets;
};

} // namespace sculk::protocol::inline abi_v975