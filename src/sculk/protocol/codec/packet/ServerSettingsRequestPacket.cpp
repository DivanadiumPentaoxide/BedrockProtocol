// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ServerSettingsRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ServerSettingsRequestPacket::getId() const noexcept {
    return MinecraftPacketIds::ServerSettingsRequest;
}

std::string_view ServerSettingsRequestPacket::getName() const noexcept { return "ServerSettingsRequestPacket"; }

void ServerSettingsRequestPacket::write(BinaryStream&) const {}

Result<> ServerSettingsRequestPacket::read(ReadOnlyBinaryStream&) { return {}; }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ServerSettingsRequestPacket::toString() const { return SCULK_FORMAT_PACKET(); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
