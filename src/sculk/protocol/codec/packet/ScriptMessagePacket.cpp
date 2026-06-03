// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ScriptMessagePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ScriptMessagePacket::getId() const noexcept { return MinecraftPacketIds::ScriptMessage; }

std::string_view ScriptMessagePacket::getName() const noexcept { return "ScriptMessagePacket"; }

void ScriptMessagePacket::write(BinaryStream& stream) const {
    stream.writeString(mMessageId);
    stream.writeString(mMessageValue);
}

Result<> ScriptMessagePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mMessageId));
    return stream.readString(mMessageValue);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ScriptMessagePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mMessageId), SCULK_FORMAT_FIELD(mMessageValue));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
