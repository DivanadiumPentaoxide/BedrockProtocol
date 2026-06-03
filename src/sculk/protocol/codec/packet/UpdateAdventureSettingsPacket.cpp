// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/UpdateAdventureSettingsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds UpdateAdventureSettingsPacket::getId() const noexcept {
    return MinecraftPacketIds::UpdateAdventureSettings;
}

std::string_view UpdateAdventureSettingsPacket::getName() const noexcept { return "UpdateAdventureSettingsPacket"; }

void UpdateAdventureSettingsPacket::write(BinaryStream& stream) const {
    stream.writeBool(mNoPvM);
    stream.writeBool(mNoMvP);
    stream.writeBool(mImmutableWorld);
    stream.writeBool(mShowNameTags);
    stream.writeBool(mAutoJump);
}

Result<> UpdateAdventureSettingsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readBool(mNoPvM));
    _SCULK_READ(stream.readBool(mNoMvP));
    _SCULK_READ(stream.readBool(mImmutableWorld));
    _SCULK_READ(stream.readBool(mShowNameTags));
    return stream.readBool(mAutoJump);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string UpdateAdventureSettingsPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mNoPvM),
        SCULK_FORMAT_FIELD(mNoMvP),
        SCULK_FORMAT_FIELD(mImmutableWorld),
        SCULK_FORMAT_FIELD(mShowNameTags),
        SCULK_FORMAT_FIELD(mAutoJump)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
