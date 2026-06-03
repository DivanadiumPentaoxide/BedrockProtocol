// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerListPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void PlayerListEntry::write(BinaryStream& stream) const {
    mUUID.write(stream);
    stream.writeVarInt64(mActorUniqueId);
    stream.writeString(mPlayerName);
    stream.writeString(mXuid);
    stream.writeString(mPlatformChatId);
    stream.writeSignedInt(mBuildPlatform);
    mSerializedSkin.write(stream);
    stream.writeBool(mIsTeacher);
    stream.writeBool(mIsHost);
    stream.writeBool(mIsSubClient);
    stream.writeSignedInt(mColor);
}

Result<> PlayerListEntry::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mUUID.read(stream));
    _SCULK_READ(stream.readVarInt64(mActorUniqueId));
    _SCULK_READ(stream.readString(mPlayerName));
    _SCULK_READ(stream.readString(mXuid));
    _SCULK_READ(stream.readString(mPlatformChatId));
    _SCULK_READ(stream.readSignedInt(mBuildPlatform));
    _SCULK_READ(mSerializedSkin.read(stream));
    _SCULK_READ(stream.readBool(mIsTeacher));
    _SCULK_READ(stream.readBool(mIsHost));
    _SCULK_READ(stream.readBool(mIsSubClient));
    return stream.readSignedInt(mColor);
}

MinecraftPacketIds PlayerListPacket::getId() const noexcept { return MinecraftPacketIds::PlayerList; }

std::string_view PlayerListPacket::getName() const noexcept { return "PlayerListPacket"; }

void PlayerListPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mAction, &BinaryStream::writeByte);
    stream.writeUnsignedVarInt(static_cast<std::uint32_t>(mPlayerEntryList.size()));
    if (mAction == ActionType::Add) {
        for (const PlayerListEntry& entry : mPlayerEntryList) {
            entry.write(stream);
        }
        for (const PlayerListEntry& entry : mPlayerEntryList) {
            stream.writeBool(entry.mSkinTrusted);
        }
    } else {
        for (const PlayerListEntry& entry : mPlayerEntryList) {
            entry.mUUID.write(stream);
        }
    }
}

Result<> PlayerListPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mAction, &ReadOnlyBinaryStream::readByte));
    std::uint32_t entryCount{};
    _SCULK_READ(stream.readUnsignedVarInt(entryCount));
    mPlayerEntryList.resize(entryCount);
    if (mAction == ActionType::Add) {
        for (PlayerListEntry& entry : mPlayerEntryList) {
            _SCULK_READ(entry.read(stream));
        }
        for (PlayerListEntry& entry : mPlayerEntryList) {
            _SCULK_READ(stream.readBool(entry.mSkinTrusted));
        }
    } else {
        for (PlayerListEntry& entry : mPlayerEntryList) {
            _SCULK_READ(entry.mUUID.read(stream));
        }
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerListPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mAction), SCULK_FORMAT_FIELD(mPlayerEntryList));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
