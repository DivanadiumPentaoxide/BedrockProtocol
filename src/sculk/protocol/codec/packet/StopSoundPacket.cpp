// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/StopSoundPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds StopSoundPacket::getId() const noexcept { return MinecraftPacketIds::StopSound; }

std::string_view StopSoundPacket::getName() const noexcept { return "StopSoundPacket"; }

void StopSoundPacket::write(BinaryStream& stream) const {
    stream.writeString(mSoundName);
    stream.writeBool(mStopAllSounds);
    stream.writeBool(mStopLegacyMusic);
}

Result<> StopSoundPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mSoundName));
    _SCULK_READ(stream.readBool(mStopAllSounds));
    return stream.readBool(mStopLegacyMusic);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string StopSoundPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mSoundName),
        SCULK_FORMAT_FIELD(mStopAllSounds),
        SCULK_FORMAT_FIELD(mStopLegacyMusic)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
