// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ItemRegistryPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ItemRegistryPacket::getId() const noexcept { return MinecraftPacketIds::ItemRegistry; }

std::string_view ItemRegistryPacket::getName() const noexcept { return "ItemRegistryPacket"; }

void ItemRegistryPacket::write(BinaryStream& stream) const { stream.writeArray(mItemData, &ItemData::write); }

Result<> ItemRegistryPacket::read(ReadOnlyBinaryStream& stream) { return stream.readArray(mItemData, &ItemData::read); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ItemRegistryPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mItemData)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
