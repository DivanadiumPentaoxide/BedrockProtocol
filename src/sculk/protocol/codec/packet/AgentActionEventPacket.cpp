// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AgentActionEventPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AgentActionEventPacket::getId() const noexcept { return MinecraftPacketIds::AgentActionEvent; }

std::string_view AgentActionEventPacket::getName() const noexcept { return "AgentActionEventPacket"; }

void AgentActionEventPacket::write(BinaryStream& stream) const {
    stream.writeString(mRequestId);
    stream.writeEnum(mAction, &BinaryStream::writeSignedInt);
    stream.writeString(mResponse);
}

Result<> AgentActionEventPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mRequestId));
    _SCULK_READ(stream.readEnum(mAction, &ReadOnlyBinaryStream::readSignedInt));
    return stream.readString(mResponse);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AgentActionEventPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mRequestId),
        SCULK_FORMAT_FIELD(mAction),
        SCULK_FORMAT_FIELD(mResponse)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
