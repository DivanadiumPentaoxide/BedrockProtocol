// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetPlayerInventoryOptionsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetPlayerInventoryOptionsPacket::getId() const noexcept {
    return MinecraftPacketIds::SetPlayerInventoryOptions;
}

std::string_view SetPlayerInventoryOptionsPacket::getName() const noexcept { return "SetPlayerInventoryOptionsPacket"; }

void SetPlayerInventoryOptionsPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mLeftInventoryTab, &BinaryStream::writeVarInt);
    stream.writeEnum(mRightInventoryTab, &BinaryStream::writeVarInt);
    stream.writeBool(mFiltering);
    stream.writeEnum(mLayoutInventory, &BinaryStream::writeVarInt);
    stream.writeEnum(mLayoutCraft, &BinaryStream::writeVarInt);
}

Result<> SetPlayerInventoryOptionsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mLeftInventoryTab, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readEnum(mRightInventoryTab, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readBool(mFiltering));
    _SCULK_READ(stream.readEnum(mLayoutInventory, &ReadOnlyBinaryStream::readVarInt));
    return stream.readEnum(mLayoutCraft, &ReadOnlyBinaryStream::readVarInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetPlayerInventoryOptionsPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mLeftInventoryTab),
        SCULK_FORMAT_FIELD(mRightInventoryTab),
        SCULK_FORMAT_FIELD(mFiltering),
        SCULK_FORMAT_FIELD(mLayoutInventory),
        SCULK_FORMAT_FIELD(mLayoutCraft)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
