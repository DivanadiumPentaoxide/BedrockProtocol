// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetScoreboardIdentityPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetScoreboardIdentityPacket::getId() const noexcept {
    return MinecraftPacketIds::SetScoreboardIdentity;
}

std::string_view SetScoreboardIdentityPacket::getName() const noexcept { return "SetScoreboardIdentityPacket"; }

void SetScoreboardIdentityPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mType, &BinaryStream::writeByte);
    stream.writeArray(mSetScoreboardIdentities, [&](const ScoreboardIdentity& data) {
        stream.writeVarInt64(data.mScoreboardId);
        if (mType == Type::Update) {
            stream.writeVarInt64(data.mPlayerUniqueId);
        }
    });
}

Result<> SetScoreboardIdentityPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
    return stream.readArray(mSetScoreboardIdentities, [&](ScoreboardIdentity& data) {
        _SCULK_READ(stream.readVarInt64(data.mScoreboardId));
        if (mType == Type::Update) {
            _SCULK_READ(stream.readVarInt64(data.mPlayerUniqueId));
        } else {
            data.mPlayerUniqueId = 0;
        }
        return Result<>{};
    });
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetScoreboardIdentityPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mType), SCULK_FORMAT_FIELD(mSetScoreboardIdentities));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
