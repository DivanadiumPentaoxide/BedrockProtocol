// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientboundAttributeLayerSyncPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientboundAttributeLayerSyncPacket::getId() const noexcept {
    return MinecraftPacketIds::ClientboundAttributeLayerSync;
}

std::string_view ClientboundAttributeLayerSyncPacket::getName() const noexcept {
    return "ClientboundAttributeLayerSyncPacket";
}

void ClientboundAttributeLayerSyncPacket::write(BinaryStream& stream) const { mData.write(stream); }

Result<> ClientboundAttributeLayerSyncPacket::read(ReadOnlyBinaryStream& stream) { return mData.read(stream); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientboundAttributeLayerSyncPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mData));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
