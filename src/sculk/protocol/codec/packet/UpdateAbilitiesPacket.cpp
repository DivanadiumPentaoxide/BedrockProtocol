// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/UpdateAbilitiesPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds UpdateAbilitiesPacket::getId() const noexcept { return MinecraftPacketIds::UpdateAbilities; }

std::string_view UpdateAbilitiesPacket::getName() const noexcept { return "UpdateAbilitiesPacket"; }

void UpdateAbilitiesPacket::write(BinaryStream& stream) const { mAbilities.write(stream); }

Result<> UpdateAbilitiesPacket::read(ReadOnlyBinaryStream& stream) { return mAbilities.read(stream); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string UpdateAbilitiesPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mAbilities)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
