// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PartyChangedPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void PartyChangedPacket::PlayerPartyInfo::write(BinaryStream& stream) const {
    stream.writeString(mPartyId);
    stream.writeBool(mIsPartyLeader);
}

Result<> PartyChangedPacket::PlayerPartyInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mPartyId));
    return stream.readBool(mIsPartyLeader);
}

MinecraftPacketIds PartyChangedPacket::getId() const noexcept { return MinecraftPacketIds::PartyChanged; }

std::string_view PartyChangedPacket::getName() const noexcept { return "PartyChangedPacket"; }

void PartyChangedPacket::write(BinaryStream& stream) const {
    stream.writeOptional(mPlayerPartyInfo, &PlayerPartyInfo::write);
}

Result<> PartyChangedPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readOptional(mPlayerPartyInfo, &PlayerPartyInfo::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PartyChangedPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mPlayerPartyInfo)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
