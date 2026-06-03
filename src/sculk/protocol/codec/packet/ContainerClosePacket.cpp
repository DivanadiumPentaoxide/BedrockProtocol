// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ContainerClosePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ContainerClosePacket::getId() const noexcept { return MinecraftPacketIds::ContainerClose; }

std::string_view ContainerClosePacket::getName() const noexcept { return "ContainerClosePacket"; }

void ContainerClosePacket::write(BinaryStream& stream) const {
    stream.writeEnum(mContainerId, &BinaryStream::writeByte);
    stream.writeEnum(mContainerType, &BinaryStream::writeByte);
    stream.writeBool(mServerInitiatedClose);
}

Result<> ContainerClosePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mContainerId, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readEnum(mContainerType, &ReadOnlyBinaryStream::readByte));
    return stream.readBool(mServerInitiatedClose);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ContainerClosePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mContainerId),
        SCULK_FORMAT_FIELD(mContainerType),
        SCULK_FORMAT_FIELD(mServerInitiatedClose)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
