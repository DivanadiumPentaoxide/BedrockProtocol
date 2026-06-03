// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ClientboundDataDrivenUICloseAllScreensPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ClientboundDataDrivenUICloseAllScreensPacket::getId() const noexcept {
    return MinecraftPacketIds::ClientboundDataDrivenUICloseAllScreens;
}

std::string_view ClientboundDataDrivenUICloseAllScreensPacket::getName() const noexcept {
    return "ClientboundDataDrivenUICloseAllScreensPacket";
}

void ClientboundDataDrivenUICloseAllScreensPacket::write(BinaryStream& stream) const {
    stream.writeOptional(mFormId, &BinaryStream::writeUnsignedInt);
}

Result<> ClientboundDataDrivenUICloseAllScreensPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readOptional(mFormId, &ReadOnlyBinaryStream::readUnsignedInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ClientboundDataDrivenUICloseAllScreensPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mFormId));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
