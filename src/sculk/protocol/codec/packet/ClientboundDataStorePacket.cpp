// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientboundDataStorePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientboundDataStorePacket::getId() const noexcept {
    return MinecraftPacketIds::ClientboundDataStore;
}

std::string_view ClientboundDataStorePacket::getName() const noexcept { return "ClientboundDataStorePacket"; }

void ClientboundDataStorePacket::write(BinaryStream& stream) const {
    stream.writeArray(mUpdates, [](BinaryStream& stream, const DataStore& update) {
        stream.writeVariant(update, [&stream](const auto& value) { value.write(stream); });
    });
}

Result<> ClientboundDataStorePacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mUpdates, [](ReadOnlyBinaryStream& stream, DataStore& update) {
        return stream.readVariant(update, [&stream](auto& value) { return value.read(stream); });
    });
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientboundDataStorePacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mUpdates)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
