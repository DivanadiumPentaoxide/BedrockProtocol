// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PhotoTransferPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PhotoTransferPacket::getId() const noexcept { return MinecraftPacketIds::PhotoTransfer; }

std::string_view PhotoTransferPacket::getName() const noexcept { return "PhotoTransferPacket"; }

void PhotoTransferPacket::write(BinaryStream& stream) const {
    stream.writeString(mPhotoName);
    stream.writeString(mPhotoData);
    stream.writeString(mBookId);
    stream.writeEnum(mType, &BinaryStream::writeByte);
    stream.writeEnum(mSourceType, &BinaryStream::writeByte);
    stream.writeSignedInt64(mOwnerId);
    stream.writeString(mNewPhotoName);
}

Result<> PhotoTransferPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mPhotoName));
    _SCULK_READ(stream.readString(mPhotoData));
    _SCULK_READ(stream.readString(mBookId));
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readEnum(mSourceType, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readSignedInt64(mOwnerId));
    return stream.readString(mNewPhotoName);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PhotoTransferPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPhotoName),
        SCULK_FORMAT_FIELD(mPhotoData),
        SCULK_FORMAT_FIELD(mBookId),
        SCULK_FORMAT_FIELD(mType),
        SCULK_FORMAT_FIELD(mSourceType),
        SCULK_FORMAT_FIELD(mOwnerId),
        SCULK_FORMAT_FIELD(mNewPhotoName)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
