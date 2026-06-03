// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientCacheStatusPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientCacheStatusPacket::getId() const noexcept { return MinecraftPacketIds::ClientCacheStatus; }

std::string_view ClientCacheStatusPacket::getName() const noexcept { return "ClientCacheStatusPacket"; }

void ClientCacheStatusPacket::write(BinaryStream& stream) const { stream.writeBool(mIsCacheSupport); }

Result<> ClientCacheStatusPacket::read(ReadOnlyBinaryStream& stream) { return stream.readBool(mIsCacheSupport); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientCacheStatusPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mIsCacheSupport));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
