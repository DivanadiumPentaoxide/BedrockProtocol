// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetSpawnPositionPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetSpawnPositionPacket::getId() const noexcept { return MinecraftPacketIds::SetSpawnPosition; }

std::string_view SetSpawnPositionPacket::getName() const noexcept { return "SetSpawnPositionPacket"; }

void SetSpawnPositionPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mSpawnPositionType, &BinaryStream::writeVarInt);
    mBlockPosition.write(stream);
    stream.writeVarInt(mDimensionType);
    mSpawnBlockPos.write(stream);
}

Result<> SetSpawnPositionPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mSpawnPositionType, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(mBlockPosition.read(stream));
    _SCULK_READ(stream.readVarInt(mDimensionType));
    return mSpawnBlockPos.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetSpawnPositionPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mSpawnPositionType),
        SCULK_FORMAT_FIELD(mBlockPosition),
        SCULK_FORMAT_FIELD(mDimensionType),
        SCULK_FORMAT_FIELD(mSpawnBlockPos)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
