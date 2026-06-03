// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetDifficultyPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetDifficultyPacket::getId() const noexcept { return MinecraftPacketIds::SetDifficulty; }

std::string_view SetDifficultyPacket::getName() const noexcept { return "SetDifficultyPacket"; }

void SetDifficultyPacket::write(BinaryStream& stream) const { stream.writeUnsignedVarInt(mDifficulty); }

Result<> SetDifficultyPacket::read(ReadOnlyBinaryStream& stream) { return stream.readUnsignedVarInt(mDifficulty); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetDifficultyPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mDifficulty)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
