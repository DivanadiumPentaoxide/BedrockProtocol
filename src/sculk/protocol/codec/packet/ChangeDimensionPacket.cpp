// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ChangeDimensionPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ChangeDimensionPacket::getId() const noexcept { return MinecraftPacketIds::ChangeDimension; }

std::string_view ChangeDimensionPacket::getName() const noexcept { return "ChangeDimensionPacket"; }

void ChangeDimensionPacket::write(BinaryStream& stream) const {
    stream.writeVarInt(mDimensionType);
    mPosition.write(stream);
    stream.writeBool(mRespawn);
    stream.writeOptional(mLoadingScreenId, &BinaryStream::writeUnsignedInt);
}

Result<> ChangeDimensionPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mDimensionType));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readBool(mRespawn));
    return stream.readOptional(mLoadingScreenId, &ReadOnlyBinaryStream::readUnsignedInt);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ChangeDimensionPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mDimensionType),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mRespawn),
        SCULK_FORMAT_FIELD(mLoadingScreenId)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
