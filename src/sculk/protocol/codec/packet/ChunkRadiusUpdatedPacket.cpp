// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ChunkRadiusUpdatedPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ChunkRadiusUpdatedPacket::getId() const noexcept { return MinecraftPacketIds::ChunkRadiusUpdated; }

std::string_view ChunkRadiusUpdatedPacket::getName() const noexcept { return "ChunkRadiusUpdatedPacket"; }

void ChunkRadiusUpdatedPacket::write(BinaryStream& stream) const { stream.writeVarInt(mChunkRadius); }

Result<> ChunkRadiusUpdatedPacket::read(ReadOnlyBinaryStream& stream) { return stream.readVarInt(mChunkRadius); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ChunkRadiusUpdatedPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mChunkRadius)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
