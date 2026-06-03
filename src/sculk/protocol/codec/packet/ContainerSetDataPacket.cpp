// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ContainerSetDataPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ContainerSetDataPacket::getId() const noexcept { return MinecraftPacketIds::ContainerSetData; }
std::string_view   ContainerSetDataPacket::getName() const noexcept { return "ContainerSetDataPacket"; }

void ContainerSetDataPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mContainerId, &BinaryStream::writeByte);
    stream.writeVarInt(mId);
    stream.writeVarInt(mValue);
}

Result<> ContainerSetDataPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mContainerId, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readVarInt(mId));
    return stream.readVarInt(mValue);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ContainerSetDataPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mContainerId), SCULK_FORMAT_FIELD(mId), SCULK_FORMAT_FIELD(mValue));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
