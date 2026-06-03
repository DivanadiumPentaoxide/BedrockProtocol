// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/UpdateClientInputLocksPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds UpdateClientInputLocksPacket::getId() const noexcept {
    return MinecraftPacketIds::UpdateClientInputLocks;
}

std::string_view UpdateClientInputLocksPacket::getName() const noexcept { return "UpdateClientInputLocksPacket"; }

void UpdateClientInputLocksPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(mInputLockComponentData);
}

Result<> UpdateClientInputLocksPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readUnsignedVarInt(mInputLockComponentData);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string UpdateClientInputLocksPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mInputLockComponentData));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
