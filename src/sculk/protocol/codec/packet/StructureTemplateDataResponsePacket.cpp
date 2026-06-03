// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/StructureTemplateDataResponsePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds StructureTemplateDataResponsePacket::getId() const noexcept {
    return MinecraftPacketIds::StructureTemplateDataResponse;
}

std::string_view StructureTemplateDataResponsePacket::getName() const noexcept {
    return "StructureTemplateDataResponsePacket";
}

void StructureTemplateDataResponsePacket::write(BinaryStream& stream) const {
    stream.writeString(mStructureName);
    stream.writeBool(mSuccess);
    if (mSuccess) {
        mNbt.write(stream);
    }
    stream.writeEnum(mResponseType, &BinaryStream::writeByte);
}

Result<> StructureTemplateDataResponsePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mStructureName));
    _SCULK_READ(stream.readBool(mSuccess));
    if (mSuccess) {
        _SCULK_READ(mNbt.read(stream));
    }
    return stream.readEnum(mResponseType, &ReadOnlyBinaryStream::readByte);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string StructureTemplateDataResponsePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mStructureName),
        SCULK_FORMAT_FIELD(mNbt),
        SCULK_FORMAT_FIELD(mResponseType)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
