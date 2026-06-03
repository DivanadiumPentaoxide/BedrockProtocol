// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientCameraAimAssistPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientCameraAimAssistPacket::getId() const noexcept {
    return MinecraftPacketIds::ClientCameraAimAssist;
}

std::string_view ClientCameraAimAssistPacket::getName() const noexcept { return "ClientCameraAimAssistPacket"; }

void ClientCameraAimAssistPacket::write(BinaryStream& stream) const {
    stream.writeString(mPresetId);
    stream.writeEnum(mAction, &BinaryStream::writeByte);
    stream.writeBool(mAllowAimAssist);
}

Result<> ClientCameraAimAssistPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mPresetId));
    _SCULK_READ(stream.readEnum(mAction, &ReadOnlyBinaryStream::readByte));
    return stream.readBool(mAllowAimAssist);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientCameraAimAssistPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPresetId),
        SCULK_FORMAT_FIELD(mAction),
        SCULK_FORMAT_FIELD(mAllowAimAssist)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
