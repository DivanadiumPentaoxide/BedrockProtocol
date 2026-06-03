// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/RequestPermissionsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds RequestPermissionsPacket::getId() const noexcept { return MinecraftPacketIds::RequestPermissions; }

std::string_view RequestPermissionsPacket::getName() const noexcept { return "RequestPermissionsPacket"; }

void RequestPermissionsPacket::write(BinaryStream& stream) const {
    stream.writeSignedInt64(mPlayerRawId);
    stream.writeVarInt(mPermissionLevel);
    stream.writeUnsignedShort(mCustomPermissionFlag);
}

Result<> RequestPermissionsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedInt64(mPlayerRawId));
    _SCULK_READ(stream.readVarInt(mPermissionLevel));
    return stream.readUnsignedShort(mCustomPermissionFlag);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string RequestPermissionsPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPlayerRawId),
        SCULK_FORMAT_FIELD(mPermissionLevel),
        SCULK_FORMAT_FIELD(mCustomPermissionFlag)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
