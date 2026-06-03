// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/RequestChunkRadiusPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds RequestChunkRadiusPacket::getId() const noexcept { return MinecraftPacketIds::RequestChunkRadius; }

std::string_view RequestChunkRadiusPacket::getName() const noexcept { return "RequestChunkRadiusPacket"; }

void RequestChunkRadiusPacket::write(BinaryStream& stream) const {
    stream.writeVarInt(mChunkRadius);
    stream.writeByte(mMaxChunkRadius);
}

Result<> RequestChunkRadiusPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mChunkRadius));
    return stream.readByte(mMaxChunkRadius);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string RequestChunkRadiusPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mChunkRadius), SCULK_FORMAT_FIELD(mMaxChunkRadius));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
