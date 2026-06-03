// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/DisconnectPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds DisconnectPacket::getId() const noexcept { return MinecraftPacketIds::Disconnect; }

std::string_view DisconnectPacket::getName() const noexcept { return "DisconnectPacket"; }

void DisconnectPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mReason, &BinaryStream::writeVarInt);
    stream.writeBool(mSkipMessage);
    if (!mSkipMessage) {
        stream.writeString(mMessage);
        stream.writeString(mFilteredMessage);
    }
}

Result<> DisconnectPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mReason, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readBool(mSkipMessage));
    if (!mSkipMessage) {
        _SCULK_READ(stream.readString(mMessage));
        _SCULK_READ(stream.readString(mFilteredMessage));
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string DisconnectPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mReason),
        SCULK_FORMAT_FIELD(mSkipMessage),
        SCULK_FORMAT_FIELD(mMessage),
        SCULK_FORMAT_FIELD(mFilteredMessage)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
