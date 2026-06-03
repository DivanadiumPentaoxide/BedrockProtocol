// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AutomationClientConnectPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AutomationClientConnectPacket::getId() const noexcept {
    return MinecraftPacketIds::AutomationClientConnect;
}

std::string_view AutomationClientConnectPacket::getName() const noexcept { return "AutomationClientConnectPacket"; }

void AutomationClientConnectPacket::write(BinaryStream& stream) const { stream.writeString(mWebSocketUrl); }

Result<> AutomationClientConnectPacket::read(ReadOnlyBinaryStream& stream) { return stream.readString(mWebSocketUrl); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AutomationClientConnectPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mWebSocketUrl));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
