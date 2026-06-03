// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientCacheBlobStatusPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientCacheBlobStatusPacket::getId() const noexcept {
    return MinecraftPacketIds::ClientCacheBlobStatus;
}

std::string_view ClientCacheBlobStatusPacket::getName() const noexcept { return "ClientCacheBlobStatusPacket"; }

void ClientCacheBlobStatusPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(static_cast<std::uint32_t>(mMissingIds.size()));
    stream.writeUnsignedVarInt(static_cast<std::uint32_t>(mFoundIds.size()));
    for (const auto& id : mMissingIds) {
        stream.writeUnsignedInt64(id);
    }
    for (const auto& id : mFoundIds) {
        stream.writeUnsignedInt64(id);
    }
}

Result<> ClientCacheBlobStatusPacket::read(ReadOnlyBinaryStream& stream) {
    std::uint32_t missingSize{};
    std::uint32_t foundSize{};
    _SCULK_READ(stream.readUnsignedVarInt(missingSize));
    _SCULK_READ(stream.readUnsignedVarInt(foundSize));
    mMissingIds.resize(missingSize);
    for (auto& id : mMissingIds) {
        _SCULK_READ(stream.readUnsignedInt64(id));
    }
    mFoundIds.resize(foundSize);
    for (auto& id : mFoundIds) {
        _SCULK_READ(stream.readUnsignedInt64(id));
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientCacheBlobStatusPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mMissingIds), SCULK_FORMAT_FIELD(mFoundIds));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
