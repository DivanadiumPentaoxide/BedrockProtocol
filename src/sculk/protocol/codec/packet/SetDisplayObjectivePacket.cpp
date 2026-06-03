// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetDisplayObjectivePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetDisplayObjectivePacket::getId() const noexcept { return MinecraftPacketIds::SetDisplayObjective; }

std::string_view SetDisplayObjectivePacket::getName() const noexcept { return "SetDisplayObjectivePacket"; }

void SetDisplayObjectivePacket::write(BinaryStream& stream) const {
    stream.writeString(mDisplaySlotName);
    stream.writeString(mObjectiveName);
    stream.writeString(mObjectiveDisplayName);
    stream.writeString(mCriteriaName);
    stream.writeEnum(mSortOrder, &BinaryStream::writeVarInt);
}

Result<> SetDisplayObjectivePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mDisplaySlotName));
    _SCULK_READ(stream.readString(mObjectiveName));
    _SCULK_READ(stream.readString(mObjectiveDisplayName));
    _SCULK_READ(stream.readString(mCriteriaName));
    return stream.readEnum(mSortOrder, &ReadOnlyBinaryStream::readVarInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetDisplayObjectivePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mDisplaySlotName),
        SCULK_FORMAT_FIELD(mObjectiveName),
        SCULK_FORMAT_FIELD(mObjectiveDisplayName),
        SCULK_FORMAT_FIELD(mCriteriaName),
        SCULK_FORMAT_FIELD(mSortOrder)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
