// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/BlockPickRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds BlockPickRequestPacket::getId() const noexcept { return MinecraftPacketIds::BlockPickRequest; }

std::string_view BlockPickRequestPacket::getName() const noexcept { return "BlockPickRequestPacket"; }

void BlockPickRequestPacket::write(BinaryStream& stream) const {
    mPosition.write(stream);
    stream.writeBool(mWithData);
    stream.writeByte(mMaxSlots);
}

Result<> BlockPickRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readBool(mWithData));
    return stream.readByte(mMaxSlots);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string BlockPickRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mWithData),
        SCULK_FORMAT_FIELD(mMaxSlots)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
