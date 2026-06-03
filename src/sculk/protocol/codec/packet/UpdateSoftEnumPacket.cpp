// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/UpdateSoftEnumPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds UpdateSoftEnumPacket::getId() const noexcept { return MinecraftPacketIds::UpdateSoftEnum; }

std::string_view UpdateSoftEnumPacket::getName() const noexcept { return "UpdateSoftEnumPacket"; }

void UpdateSoftEnumPacket::write(BinaryStream& stream) const {
    stream.writeString(mEnumName);
    stream.writeArray(mEnumValues, &BinaryStream::writeString);
    stream.writeEnum(mUpdateType, &BinaryStream::writeByte);
}

Result<> UpdateSoftEnumPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mEnumName));
    _SCULK_READ(stream.readArray(mEnumValues, &ReadOnlyBinaryStream::readString));
    return stream.readEnum(mUpdateType, &ReadOnlyBinaryStream::readByte);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string UpdateSoftEnumPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mEnumName),
        SCULK_FORMAT_FIELD(mEnumValues),
        SCULK_FORMAT_FIELD(mUpdateType)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
