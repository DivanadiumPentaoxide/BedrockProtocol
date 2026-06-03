// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ModalFormResponsePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ModalFormResponsePacket::getId() const noexcept { return MinecraftPacketIds::ModalFormResponse; }

std::string_view ModalFormResponsePacket::getName() const noexcept { return "ModalFormResponsePacket"; }

void ModalFormResponsePacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(mFormId);
    stream.writeOptional(mResponse, &BinaryStream::writeString);
    stream.writeOptional(mCancelReason, [](BinaryStream& stream, const CancelReason& reason) {
        stream.writeEnum(reason, &BinaryStream::writeByte);
    });
}

Result<> ModalFormResponsePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt(mFormId));
    _SCULK_READ(stream.readOptional(mResponse, &ReadOnlyBinaryStream::readString));
    return stream.readOptional(mCancelReason, [](ReadOnlyBinaryStream& stream, CancelReason& reason) {
        return stream.readEnum(reason, &ReadOnlyBinaryStream::readByte);
    });
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ModalFormResponsePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mFormId),
        SCULK_FORMAT_FIELD(mResponse),
        SCULK_FORMAT_FIELD(mCancelReason)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
