// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AwardAchievementPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AwardAchievementPacket::getId() const noexcept { return MinecraftPacketIds::AwardAchievement; }

std::string_view AwardAchievementPacket::getName() const noexcept { return "AwardAchievementPacket"; }

void AwardAchievementPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mAchievementId, &BinaryStream::writeSignedInt);
}

Result<> AwardAchievementPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readEnum(mAchievementId, &ReadOnlyBinaryStream::readSignedInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AwardAchievementPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mAchievementId)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
