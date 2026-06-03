// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetLocalPlayerAsInitializedPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetLocalPlayerAsInitializedPacket::getId() const noexcept {
    return MinecraftPacketIds::SetLocalPlayerAsInitialized;
}

std::string_view SetLocalPlayerAsInitializedPacket::getName() const noexcept {
    return "SetLocalPlayerAsInitializedPacket";
}

void SetLocalPlayerAsInitializedPacket::write(BinaryStream& stream) const { stream.writeUnsignedVarInt64(mRuntimeId); }

Result<> SetLocalPlayerAsInitializedPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readUnsignedVarInt64(mRuntimeId);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetLocalPlayerAsInitializedPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mRuntimeId));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
