// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ServerStatsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ServerStatsPacket::getId() const noexcept { return MinecraftPacketIds::ServerStats; }

std::string_view ServerStatsPacket::getName() const noexcept { return "ServerStatsPacket"; }

void ServerStatsPacket::write(BinaryStream& stream) const {
    stream.writeFloat(mServerTime);
    stream.writeFloat(mNetworkTime);
}

Result<> ServerStatsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readFloat(mServerTime));
    return stream.readFloat(mNetworkTime);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ServerStatsPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mServerTime), SCULK_FORMAT_FIELD(mNetworkTime));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
