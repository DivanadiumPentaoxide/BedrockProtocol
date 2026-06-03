// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/LessonProgressPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds LessonProgressPacket::getId() const noexcept { return MinecraftPacketIds::LessonProgress; }

std::string_view LessonProgressPacket::getName() const noexcept { return "LessonProgressPacket"; }

void LessonProgressPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mAction, &BinaryStream::writeVarInt);
    stream.writeVarInt(mScore);
    stream.writeString(mActivityId);
}

Result<> LessonProgressPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mAction, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(stream.readVarInt(mScore));
    return stream.readString(mActivityId);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LessonProgressPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mAction),
        SCULK_FORMAT_FIELD(mScore),
        SCULK_FORMAT_FIELD(mActivityId)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
