// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <RakNetTypes.h>
#include <cstdint>

namespace sculk::protocol::inline abi_v975 {

enum class NetworkEventType : std::uint8_t {
    Connected,
    Disconnected,
    ConnectionFailed,
};

struct NetworkEvent {
    NetworkEventType      mType{NetworkEventType::Disconnected};
    RakNet::RakNetGUID    mGuid{RakNet::UNASSIGNED_RAKNET_GUID};
    RakNet::SystemAddress mAddress{RakNet::UNASSIGNED_SYSTEM_ADDRESS};
};

} // namespace sculk::protocol::inline abi_v975
