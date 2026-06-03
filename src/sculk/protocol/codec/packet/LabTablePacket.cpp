// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/LabTablePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds LabTablePacket::getId() const noexcept { return MinecraftPacketIds::LabTable; }

std::string_view LabTablePacket::getName() const noexcept { return "LabTablePacket"; }

void LabTablePacket::write(BinaryStream& stream) const {
    stream.writeEnum(mType, &BinaryStream::writeByte);
    mPosition.write(stream);
    stream.writeEnum(mReaction, &BinaryStream::writeByte);
}

Result<> LabTablePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(mPosition.read(stream));
    return stream.readEnum(mReaction, &ReadOnlyBinaryStream::readByte);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LabTablePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mType), SCULK_FORMAT_FIELD(mPosition), SCULK_FORMAT_FIELD(mReaction));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
