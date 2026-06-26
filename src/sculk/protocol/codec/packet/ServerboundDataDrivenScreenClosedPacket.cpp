// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ServerboundDataDrivenScreenClosedPacket.hpp"
#include "../utility/EnumName.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ServerboundDataDrivenScreenClosedPacket::getId() const noexcept {
    return MinecraftPacketIds::ServerboundDataDrivenScreenClosed;
}

std::string_view ServerboundDataDrivenScreenClosedPacket::getName() const noexcept {
    return "ServerboundDataDrivenScreenClosedPacket";
}

void ServerboundDataDrivenScreenClosedPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(mFormId);
    utils::writeEnumName(stream, mCloseReason);
}

Result<> ServerboundDataDrivenScreenClosedPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt(mFormId));
    return utils::readEnumName(stream, mCloseReason);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ServerboundDataDrivenScreenClosedPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mFormId), SCULK_FORMAT_FIELD(mCloseReason));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
