// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AddVolumeEntityPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AddVolumeEntityPacket::getId() const noexcept { return MinecraftPacketIds::AddVolumeEntity; }

std::string_view AddVolumeEntityPacket::getName() const noexcept { return "AddVolumeEntityPacket"; }

void AddVolumeEntityPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(mEntityNetId);
    mComponents.write(stream);
    stream.writeString(mJsonIdentifier);
    stream.writeString(mInstanceName);
    mMaxBounds.write(stream);
    mMaxBounds.write(stream);
    stream.writeVarInt(mDimensionType);
    stream.writeString(mEngineVersion);
}

Result<> AddVolumeEntityPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt(mEntityNetId));
    _SCULK_READ(mComponents.read(stream));
    _SCULK_READ(stream.readString(mJsonIdentifier));
    _SCULK_READ(stream.readString(mInstanceName));
    _SCULK_READ(mMaxBounds.read(stream));
    _SCULK_READ(mMaxBounds.read(stream));
    _SCULK_READ(stream.readVarInt(mDimensionType));
    return stream.readString(mEngineVersion);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AddVolumeEntityPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mEntityNetId),
        SCULK_FORMAT_FIELD(mComponents),
        SCULK_FORMAT_FIELD(mJsonIdentifier),
        SCULK_FORMAT_FIELD(mInstanceName),
        SCULK_FORMAT_FIELD(mMinBounds),
        SCULK_FORMAT_FIELD(mMaxBounds),
        SCULK_FORMAT_FIELD(mDimensionType),
        SCULK_FORMAT_FIELD(mEngineVersion)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
