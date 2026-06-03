// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ResourcePackDataInfoPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ResourcePackDataInfoPacket::getId() const noexcept {
    return MinecraftPacketIds::ResourcePackDataInfo;
}

std::string_view ResourcePackDataInfoPacket::getName() const noexcept { return "ResourcePackDataInfoPacket"; }

void ResourcePackDataInfoPacket::write(BinaryStream& stream) const {
    stream.writeString(mResourceName);
    stream.writeUnsignedInt(mChunkSize);
    stream.writeUnsignedInt(mChunkIndex);
    stream.writeUnsignedInt64(mFileSize);
    stream.writeString(mFileHash);
    stream.writeBool(mIsPremiumPack);
    stream.writeEnum(mPackType, &BinaryStream::writeByte);
}

Result<> ResourcePackDataInfoPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mResourceName));
    _SCULK_READ(stream.readUnsignedInt(mChunkSize));
    _SCULK_READ(stream.readUnsignedInt(mChunkIndex));
    _SCULK_READ(stream.readUnsignedInt64(mFileSize));
    _SCULK_READ(stream.readString(mFileHash));
    _SCULK_READ(stream.readBool(mIsPremiumPack));
    return stream.readEnum(mPackType, &ReadOnlyBinaryStream::readByte);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ResourcePackDataInfoPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mResourceName),
        SCULK_FORMAT_FIELD(mChunkSize),
        SCULK_FORMAT_FIELD(mChunkIndex),
        SCULK_FORMAT_FIELD(mFileSize),
        SCULK_FORMAT_FIELD(mFileHash),
        SCULK_FORMAT_FIELD(mIsPremiumPack),
        SCULK_FORMAT_FIELD(mPackType)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
