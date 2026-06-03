// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CompletedUsingItemPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CompletedUsingItemPacket::getId() const noexcept { return MinecraftPacketIds::CompletedUsingItem; }

std::string_view CompletedUsingItemPacket::getName() const noexcept { return "CompletedUsingItemPacket"; }

void CompletedUsingItemPacket::write(BinaryStream& stream) const {
    stream.writeSignedShort(mItemId);
    stream.writeSignedInt(mUseMethod);
}

Result<> CompletedUsingItemPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedShort(mItemId));
    return stream.readSignedInt(mUseMethod);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CompletedUsingItemPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mItemId), SCULK_FORMAT_FIELD(mUseMethod));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
