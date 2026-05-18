// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientboundDebugRendererPacket.hpp"
#include "../utility/EnumName.hpp"

namespace sculk::protocol::inline abi_v975 {

void ClientboundDebugRendererPacket::DebugMarkerData::write(BinaryStream& stream) const {
    stream.writeString(mText);
    mPosition.write(stream);
    stream.writeSignedInt(mColor);
    stream.writeUnsignedInt64(mDurationMilliseconds);
}

Result<> ClientboundDebugRendererPacket::DebugMarkerData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mText));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readSignedInt(mColor));
    return stream.readUnsignedInt64(mDurationMilliseconds);
}

MinecraftPacketIds ClientboundDebugRendererPacket::getId() const noexcept {
    return MinecraftPacketIds::ClientboundDebugRenderer;
}

std::string_view ClientboundDebugRendererPacket::getName() const noexcept { return "ClientboundDebugRendererPacket"; }

void ClientboundDebugRendererPacket::write(BinaryStream& stream) const {
    utils::writeEnumName(stream, mType);
    stream.writeOptional(mDebugMarkerData, &DebugMarkerData::write);
}

Result<> ClientboundDebugRendererPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(utils::readEnumName(stream, mType));
    return stream.readOptional(mDebugMarkerData, &DebugMarkerData::read);
}

} // namespace sculk::protocol::inline abi_v975
