// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ResourcePackChunkRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ResourcePackChunkRequestPacket::getId() const noexcept {
    return MinecraftPacketIds::ResourcePackChunkRequest;
}

std::string_view ResourcePackChunkRequestPacket::getName() const noexcept { return "ResourcePackChunkRequestPacket"; }

void ResourcePackChunkRequestPacket::write(BinaryStream& stream) const {
    stream.writeString(mResourceName);
    stream.writeUnsignedInt(mChunkIndex);
}

Result<> ResourcePackChunkRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mResourceName));
    return stream.readUnsignedInt(mChunkIndex);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ResourcePackChunkRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mResourceName), SCULK_FORMAT_FIELD(mChunkIndex));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
