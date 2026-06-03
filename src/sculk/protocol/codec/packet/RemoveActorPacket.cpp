// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/RemoveActorPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds RemoveActorPacket::getId() const noexcept { return MinecraftPacketIds::RemoveActor; }

std::string_view RemoveActorPacket::getName() const noexcept { return "RemoveActorPacket"; }

void RemoveActorPacket::write(BinaryStream& stream) const { stream.writeVarInt64(mActorUniqueId); }

Result<> RemoveActorPacket::read(ReadOnlyBinaryStream& stream) { return stream.readVarInt64(mActorUniqueId); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string RemoveActorPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mActorUniqueId)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
