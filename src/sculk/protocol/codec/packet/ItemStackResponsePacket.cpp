// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ItemStackResponsePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ItemStackResponsePacket::getId() const noexcept { return MinecraftPacketIds::ItemStackResponse; }

std::string_view ItemStackResponsePacket::getName() const noexcept { return "ItemStackResponsePacket"; }

void ItemStackResponsePacket::write(BinaryStream& stream) const { mResponse.write(stream); }

Result<> ItemStackResponsePacket::read(ReadOnlyBinaryStream& stream) { return mResponse.read(stream); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ItemStackResponsePacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mResponse)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
