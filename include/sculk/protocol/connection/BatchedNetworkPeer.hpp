// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "BatchedPackets.hpp"
#include "RakPeerInterface.h"

namespace sculk::protocol::inline abi_v975 {

class ISession;

enum class CompressionType : std::int8_t {
    None   = -1,
    Zlib   = 0,
    Snappy = 1,
};

class BatchedNetworkPeer {
    bool                      mIsCompressed{};
    CompressionType           mCompressionAlgorithm{};
    bool                      mIsEncrypted{};
    RakNet::RakPeerInterface* mRakPeer{};

    friend class ISession;

public:
    // TODO: Implement the constructor

    ~BatchedNetworkPeer();

public:
    [[nodiscard]] constexpr bool isCompressed() const { return mIsCompressed; }

    [[nodiscard]] constexpr bool isEncrypted() const { return mIsEncrypted; }

    void sendBatchedPackets(const BatchedPackets& packets) const;
};

} // namespace sculk::protocol::inline abi_v975