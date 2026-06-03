// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/TransferPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds TransferPacket::getId() const noexcept { return MinecraftPacketIds::Transfer; }

std::string_view TransferPacket::getName() const noexcept { return "TransferPacket"; }

void TransferPacket::write(BinaryStream& stream) const {
    stream.writeString(mServerAddress);
    stream.writeUnsignedShort(mServerPort);
    stream.writeBool(mReloadWorld);
}

Result<> TransferPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mServerAddress));
    _SCULK_READ(stream.readUnsignedShort(mServerPort));
    return stream.readBool(mReloadWorld);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string TransferPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mServerAddress),
        SCULK_FORMAT_FIELD(mServerPort),
        SCULK_FORMAT_FIELD(mReloadWorld)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
