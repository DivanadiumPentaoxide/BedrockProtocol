// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/DeathInfoPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds DeathInfoPacket::getId() const noexcept { return MinecraftPacketIds::DeathInfo; }

std::string_view DeathInfoPacket::getName() const noexcept { return "DeathInfoPacket"; }

void DeathInfoPacket::write(BinaryStream& stream) const {
    stream.writeString(mDeathCauseAttackName);
    stream.writeArray(mDeathCauseMessageList, &BinaryStream::writeString);
}

Result<> DeathInfoPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mDeathCauseAttackName));
    return stream.readArray(mDeathCauseMessageList, &ReadOnlyBinaryStream::readString);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string DeathInfoPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mDeathCauseAttackName), SCULK_FORMAT_FIELD(mDeathCauseMessageList));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
