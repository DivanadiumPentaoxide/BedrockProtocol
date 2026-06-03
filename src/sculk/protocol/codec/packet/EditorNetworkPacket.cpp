// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/EditorNetworkPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds EditorNetworkPacket::getId() const noexcept { return MinecraftPacketIds::EditorNetwork; }

std::string_view EditorNetworkPacket::getName() const noexcept { return "EditorNetworkPacket"; }

void EditorNetworkPacket::write(BinaryStream& stream) const {
    stream.writeBool(mRouteToManager);
    stream.writeString(mRawVariantName);
    stream.writeString(mRawVariantData);
}

Result<> EditorNetworkPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readBool(mRouteToManager));
    _SCULK_READ(stream.readString(mRawVariantName));
    return stream.readString(mRawVariantData);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string EditorNetworkPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mRouteToManager),
        SCULK_FORMAT_FIELD(mRawVariantName),
        SCULK_FORMAT_FIELD(mRawVariantData)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
