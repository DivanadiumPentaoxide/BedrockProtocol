// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/StructureTemplateDataRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds StructureTemplateDataRequestPacket::getId() const noexcept {
    return MinecraftPacketIds::StructureTemplateDataRequest;
}

std::string_view StructureTemplateDataRequestPacket::getName() const noexcept {
    return "StructureTemplateDataRequestPacket";
}

void StructureTemplateDataRequestPacket::write(BinaryStream& stream) const {
    stream.writeString(mStructureName);
    mPosition.write(stream);
    mSettings.write(stream);
    stream.writeEnum(mOperationType, &BinaryStream::writeByte);
}

Result<> StructureTemplateDataRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mStructureName));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(mSettings.read(stream));
    return stream.readEnum(mOperationType, &ReadOnlyBinaryStream::readByte);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string StructureTemplateDataRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mStructureName),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mSettings),
        SCULK_FORMAT_FIELD(mOperationType)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
