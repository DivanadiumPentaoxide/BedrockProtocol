// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SetScorePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SetScorePacket::getId() const noexcept { return MinecraftPacketIds::SetScore; }

std::string_view SetScorePacket::getName() const noexcept { return "SetScorePacket"; }

void SetScorePacket::write(BinaryStream& stream) const {
    stream.writeEnum(mPacketType, &BinaryStream::writeByte);
    stream.writeUnsignedVarInt(static_cast<std::uint32_t>(mScoresInfo.size()));
    for (const auto& info : mScoresInfo) {
        stream.writeVarInt64(info.mScoreboardId);
        stream.writeString(info.mObjectiveName);
        stream.writeSignedInt(info.mScoreValue);
        if (mPacketType == PacketType::Change) {
            stream.writeEnum(info.mIdentityType, &BinaryStream::writeByte);
            switch (info.mIdentityType) {
            case IdentityType::Player:
            case IdentityType::Entity:
                stream.writeVarInt64(info.mActorUniqueId);
                break;
            case IdentityType::FakePlayer:
                stream.writeString(info.mFakePlayerName);
                break;
            default:
                break;
            }
        }
    }
}

Result<> SetScorePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mPacketType, &ReadOnlyBinaryStream::readByte));
    std::uint32_t count{};
    _SCULK_READ(stream.readUnsignedVarInt(count));
    mScoresInfo.clear();
    mScoresInfo.resize(count);
    for (auto& info : mScoresInfo) {
        _SCULK_READ(stream.readVarInt64(info.mScoreboardId));
        _SCULK_READ(stream.readString(info.mObjectiveName));
        _SCULK_READ(stream.readSignedInt(info.mScoreValue));
        if (mPacketType == PacketType::Change) {
            _SCULK_READ(stream.readEnum(info.mIdentityType, &ReadOnlyBinaryStream::readByte));
            switch (info.mIdentityType) {
            case IdentityType::Player:
            case IdentityType::Entity:
                _SCULK_READ(stream.readVarInt64(info.mActorUniqueId));
                break;
            case IdentityType::FakePlayer:
                _SCULK_READ(stream.readString(info.mFakePlayerName));
                break;
            default:
                break;
            }
        }
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SetScorePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mPacketType), SCULK_FORMAT_FIELD(mScoresInfo));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
