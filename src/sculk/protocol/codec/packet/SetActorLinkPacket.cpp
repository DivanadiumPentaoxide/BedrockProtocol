// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetActorLinkPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetActorLinkPacket::getId() const noexcept { return MinecraftPacketIds::SetActorLink; }

std::string_view SetActorLinkPacket::getName() const noexcept { return "SetActorLinkPacket"; }

void SetActorLinkPacket::write(BinaryStream& stream) const { mLink.write(stream); }

Result<> SetActorLinkPacket::read(ReadOnlyBinaryStream& stream) { return mLink.read(stream); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetActorLinkPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mLink)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
