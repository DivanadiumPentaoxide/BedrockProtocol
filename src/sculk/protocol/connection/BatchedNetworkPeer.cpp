// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/BatchedNetworkPeer.hpp"
#include "BitStream.h"
#include "compression/Snappy.hpp"
#include "compression/Zlib.hpp"

namespace sculk::protocol::inline abi_v975 {

constexpr std::uint8_t MINECRAFT_PACKET_RAKNET_ID = 0xFE;

BatchedNetworkPeer::~BatchedNetworkPeer() {
    if (mRakPeer) {
        RakNet::RakPeerInterface::DestroyInstance(mRakPeer);
        mRakPeer = nullptr;
    }
}

void BatchedNetworkPeer::sendBatchedPackets(const BatchedPackets& packets) const {
    std::vector<std::byte> buffer{};
    BinaryStream           stream{buffer};

    std::vector<std::byte> packetsBuffer{};
    BinaryStream           packetsStream{packetsBuffer};
    packets.serialize(packetsStream);

    if (isCompressed()) {
        switch (mCompressionAlgorithm) {
        case CompressionType::Zlib:
            packetsBuffer = compression::zlib::compress(packetsBuffer);
            break;
        case CompressionType::Snappy:
            packetsBuffer = compression::snappy::compress(packetsBuffer);
            break;
        default:
            break;
        }
        stream.writeByte(static_cast<std::uint8_t>(mCompressionAlgorithm));
    }
    stream.writeAndMoveBuffer(std::move(packetsBuffer));

    std::vector<std::byte> finalBuffer{};
    BinaryStream           finalStream{finalBuffer};
    finalStream.writeByte(MINECRAFT_PACKET_RAKNET_ID);
    if (isEncrypted()) {
        // TODO: Handle encryption
        // buffer = encrypt(buffer);
    }
    finalStream.writeAndMoveBuffer(std::move(buffer));

    // TODO: send buffer with RakNetPeer
    RakNet::BitStream raknetStream{
        reinterpret_cast<std::uint8_t*>(finalBuffer.data()),
        static_cast<std::uint32_t>(finalBuffer.size()),
        false
    };
    mRakPeer->Send(&raknetStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, false);
}

} // namespace sculk::protocol::inline abi_v975