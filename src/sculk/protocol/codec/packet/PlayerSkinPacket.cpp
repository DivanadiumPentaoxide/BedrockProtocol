// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerSkinPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerSkinPacket::getId() const noexcept { return MinecraftPacketIds::PlayerSkin; }

std::string_view PlayerSkinPacket::getName() const noexcept { return "PlayerSkinPacket"; }

void PlayerSkinPacket::write(BinaryStream& stream) const {
    mUUID.write(stream);
    mSerializedSkin.write(stream);
    stream.writeString(mNewSkinName);
    stream.writeString(mOldSkinName);
    stream.writeBool(mIsTrustedSkin);
}

Result<> PlayerSkinPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mUUID.read(stream));
    _SCULK_READ(mSerializedSkin.read(stream));
    _SCULK_READ(stream.readString(mNewSkinName));
    _SCULK_READ(stream.readString(mOldSkinName));
    return stream.readBool(mIsTrustedSkin);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerSkinPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mUUID),
        SCULK_FORMAT_FIELD(mSerializedSkin),
        SCULK_FORMAT_FIELD(mNewSkinName),
        SCULK_FORMAT_FIELD(mOldSkinName)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
