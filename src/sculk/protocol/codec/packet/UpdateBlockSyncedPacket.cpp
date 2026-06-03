// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/UpdateBlockSyncedPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds UpdateBlockSyncedPacket::getId() const noexcept { return MinecraftPacketIds::UpdateBlockSynced; }

std::string_view UpdateBlockSyncedPacket::getName() const noexcept { return "UpdateBlockSyncedPacket"; }

void UpdateBlockSyncedPacket::write(BinaryStream& stream) const {
    mBlockPosition.write(stream);
    stream.writeUnsignedVarInt(mRuntimeId);
    stream.writeUnsignedVarInt(mFlag);
    stream.writeUnsignedVarInt(mLayer);
    stream.writeUnsignedVarInt64(mUniqueActorId);
    stream.writeUnsignedVarInt64(mActorSyncMessage);
}

Result<> UpdateBlockSyncedPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mBlockPosition.read(stream));
    _SCULK_READ(stream.readUnsignedVarInt(mRuntimeId));
    _SCULK_READ(stream.readUnsignedVarInt(mFlag));
    _SCULK_READ(stream.readUnsignedVarInt(mLayer));
    _SCULK_READ(stream.readUnsignedVarInt64(mUniqueActorId));
    return stream.readUnsignedVarInt64(mActorSyncMessage);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string UpdateBlockSyncedPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mBlockPosition),
        SCULK_FORMAT_FIELD(mRuntimeId),
        SCULK_FORMAT_FIELD(mFlag),
        SCULK_FORMAT_FIELD(mLayer),
        SCULK_FORMAT_FIELD(mUniqueActorId),
        SCULK_FORMAT_FIELD(mActorSyncMessage)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
