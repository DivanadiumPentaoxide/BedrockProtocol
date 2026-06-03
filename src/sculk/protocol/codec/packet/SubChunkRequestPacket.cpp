// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SubChunkRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void SubChunkRequestPacket::SubChunkPosOffset::write(BinaryStream& stream) const {
    stream.writeSignedChar(mX);
    stream.writeSignedChar(mY);
    stream.writeSignedChar(mZ);
}

Result<> SubChunkRequestPacket::SubChunkPosOffset::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedChar(mX));
    _SCULK_READ(stream.readSignedChar(mY));
    return stream.readSignedChar(mZ);
}

MinecraftPacketIds SubChunkRequestPacket::getId() const noexcept { return MinecraftPacketIds::SubChunkRequest; }

std::string_view SubChunkRequestPacket::getName() const noexcept { return "SubChunkRequestPacket"; }

void SubChunkRequestPacket::write(BinaryStream& stream) const {
    stream.writeVarInt(mDimensionType);
    mCenterPos.write(stream);
    stream.writeUnsignedInt(static_cast<std::uint32_t>(mSubChunkPosOffsetList.size()));
    for (const SubChunkPosOffset& offset : mSubChunkPosOffsetList) {
        offset.write(stream);
    }
}

Result<> SubChunkRequestPacket::read(ReadOnlyBinaryStream& stream) {
    std::uint32_t size{};
    _SCULK_READ(stream.readVarInt(mDimensionType));
    _SCULK_READ(mCenterPos.read(stream));
    _SCULK_READ(stream.readUnsignedInt(size));
    mSubChunkPosOffsetList.resize(size);
    for (SubChunkPosOffset& offset : mSubChunkPosOffsetList) {
        _SCULK_READ(offset.read(stream));
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SubChunkRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mDimensionType),
        SCULK_FORMAT_FIELD(mCenterPos),
        SCULK_FORMAT_FIELD(mSubChunkPosOffsetList)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
