// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ServerboundLoadingScreenPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ServerboundLoadingScreenPacket::getId() const noexcept {
    return MinecraftPacketIds::ServerboundLoadingScreen;
}

std::string_view ServerboundLoadingScreenPacket::getName() const noexcept { return "ServerboundLoadingScreenPacket"; }

void ServerboundLoadingScreenPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mType, &BinaryStream::writeVarInt);
    stream.writeOptional(mLoadingScreenId, &BinaryStream::writeUnsignedInt);
}

Result<> ServerboundLoadingScreenPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readVarInt));
    return stream.readOptional(mLoadingScreenId, &ReadOnlyBinaryStream::readUnsignedInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ServerboundLoadingScreenPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mType), SCULK_FORMAT_FIELD(mLoadingScreenId));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
