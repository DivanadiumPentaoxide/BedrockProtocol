// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CommandRequestPacket.hpp"
#include "../utility/EnumName.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CommandRequestPacket::getId() const noexcept { return MinecraftPacketIds::CommandRequest; }

std::string_view CommandRequestPacket::getName() const noexcept { return "CommandRequestPacket"; }

void CommandRequestPacket::write(BinaryStream& stream) const {
    stream.writeString(mCommand);
    mOriginData.write(stream);
    stream.writeBool(mIsInternal);
    utils::writeEnumName(stream, mVersion);
}

Result<> CommandRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mCommand));
    _SCULK_READ(mOriginData.read(stream));
    _SCULK_READ(stream.readBool(mIsInternal));
    return utils::readEnumName(stream, mVersion);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CommandRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mCommand),
        SCULK_FORMAT_FIELD(mOriginData),
        SCULK_FORMAT_FIELD(mIsInternal)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
