// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientCacheMissResponsePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ClientCacheMissResponsePacket::ClientCache::write(BinaryStream& stream) const {
    stream.writeUnsignedInt64(mBlobId);
    stream.writeString(mBlobData);
}

Result<> ClientCacheMissResponsePacket::ClientCache::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt64(mBlobId));
    return stream.readString(mBlobData);
}

MinecraftPacketIds ClientCacheMissResponsePacket::getId() const noexcept {
    return MinecraftPacketIds::ClientCacheMissResponse;
}

std::string_view ClientCacheMissResponsePacket::getName() const noexcept { return "ClientCacheMissResponsePacket"; }

void ClientCacheMissResponsePacket::write(BinaryStream& stream) const {
    stream.writeArray(mMissingBlobs, &ClientCache::write);
}

Result<> ClientCacheMissResponsePacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mMissingBlobs, &ClientCache::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientCacheMissResponsePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mMissingBlobs));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
