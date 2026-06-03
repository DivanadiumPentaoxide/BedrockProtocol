// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CommandBlockUpdatePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CommandBlockUpdatePacket::getId() const noexcept { return MinecraftPacketIds::CommandBlockUpdate; }

std::string_view CommandBlockUpdatePacket::getName() const noexcept { return "CommandBlockUpdatePacket"; }

void CommandBlockUpdatePacket::write(BinaryStream& stream) const {
    stream.writeBool(mIsBlock);
    if (mIsBlock) {
        mBlockPosition.write(stream);
        stream.writeUnsignedVarInt(mCommandBlockMode);
        stream.writeBool(mRedstoneMode);
        stream.writeBool(mConditional);
    } else {
        stream.writeUnsignedVarInt64(mActorRuntimeId);
    }
    stream.writeString(mCommand);
    stream.writeString(mLastOutput);
    stream.writeString(mName);
    stream.writeString(mFilteredName);
    stream.writeBool(mTrackOutput);
    stream.writeUnsignedInt(mTickDelay);
    stream.writeBool(mExecuteOnFirstTick);
}

Result<> CommandBlockUpdatePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readBool(mIsBlock));
    if (mIsBlock) {
        _SCULK_READ(mBlockPosition.read(stream));
        _SCULK_READ(stream.readUnsignedVarInt(mCommandBlockMode));
        _SCULK_READ(stream.readBool(mRedstoneMode));
        _SCULK_READ(stream.readBool(mConditional));
    } else {
        _SCULK_READ(stream.readUnsignedVarInt64(mActorRuntimeId));
    }
    _SCULK_READ(stream.readString(mCommand));
    _SCULK_READ(stream.readString(mLastOutput));
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readString(mFilteredName));
    _SCULK_READ(stream.readBool(mTrackOutput));
    _SCULK_READ(stream.readUnsignedInt(mTickDelay));
    return stream.readBool(mExecuteOnFirstTick);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CommandBlockUpdatePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mIsBlock),
        SCULK_FORMAT_FIELD(mActorRuntimeId),
        SCULK_FORMAT_FIELD(mBlockPosition),
        SCULK_FORMAT_FIELD(mCommandBlockMode),
        SCULK_FORMAT_FIELD(mRedstoneMode),
        SCULK_FORMAT_FIELD(mConditional),
        SCULK_FORMAT_FIELD(mCommand),
        SCULK_FORMAT_FIELD(mLastOutput),
        SCULK_FORMAT_FIELD(mName),
        SCULK_FORMAT_FIELD(mFilteredName),
        SCULK_FORMAT_FIELD(mTrackOutput),
        SCULK_FORMAT_FIELD(mTickDelay),
        SCULK_FORMAT_FIELD(mExecuteOnFirstTick)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
