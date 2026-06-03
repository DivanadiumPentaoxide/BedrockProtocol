// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CreatePhotoPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CreatePhotoPacket::getId() const noexcept { return MinecraftPacketIds::CreatePhoto; }

std::string_view CreatePhotoPacket::getName() const noexcept { return "CreatePhotoPacket"; }

void CreatePhotoPacket::write(BinaryStream& stream) const {
    stream.writeUnsignedInt64(mRawId);
    stream.writeString(mPhotoName);
    stream.writeString(mPhotoItemName);
}

Result<> CreatePhotoPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt64(mRawId));
    _SCULK_READ(stream.readString(mPhotoName));
    return stream.readString(mPhotoItemName);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CreatePhotoPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mRawId),
        SCULK_FORMAT_FIELD(mPhotoName),
        SCULK_FORMAT_FIELD(mPhotoItemName)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
