// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/VoxelShapesPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void VoxelShapesPacket::RegistryHandle::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeUnsignedShort(mValue);
}

Result<> VoxelShapesPacket::RegistryHandle::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    return stream.readUnsignedShort(mValue);
}

MinecraftPacketIds VoxelShapesPacket::getId() const noexcept { return MinecraftPacketIds::VoxelShapes; }

std::string_view VoxelShapesPacket::getName() const noexcept { return "VoxelShapesPacket"; }

void VoxelShapesPacket::write(BinaryStream& stream) const {
    stream.writeArray(mShapes, &SerializableVoxelShape::write);
    stream.writeArray(mRegistryHandles, &RegistryHandle::write);
    stream.writeUnsignedShort(mCustomShapeCount);
}

Result<> VoxelShapesPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mShapes, &SerializableVoxelShape::read));
    _SCULK_READ(stream.readArray(mRegistryHandles, &RegistryHandle::read));
    return stream.readUnsignedShort(mCustomShapeCount);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string VoxelShapesPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mShapes),
        SCULK_FORMAT_FIELD(mRegistryHandles),
        SCULK_FORMAT_FIELD(mCustomShapeCount)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
