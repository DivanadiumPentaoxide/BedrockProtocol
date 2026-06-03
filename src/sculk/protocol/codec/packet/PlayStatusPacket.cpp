// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayStatusPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayStatusPacket::getId() const noexcept { return MinecraftPacketIds::PlayStatus; }

std::string_view PlayStatusPacket::getName() const noexcept { return "PlayStatusPacket"; }

void PlayStatusPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mStatus, &BinaryStream::writeSignedBigEndianInt);
}

Result<> PlayStatusPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readEnum(mStatus, &ReadOnlyBinaryStream::readSignedBigEndianInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayStatusPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mStatus)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
