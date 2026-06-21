// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/TextPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds TextPacket::getId() const noexcept { return MinecraftPacketIds::Text; }

std::string_view TextPacket::getName() const noexcept { return "TextPacket"; }

void TextPacket::write(BinaryStream& stream) const {
    stream.writeBool(mLocalize);
    stream.writeVariant(
        mBody,
        Overload{
            [&](const TextPacket::MessageOnly& body) {
                stream.writeEnum(mType, &BinaryStream::writeByte);
                stream.writeString(body.mMessage);
            },
            [&](const TextPacket::AuthorAndMessage& body) {
                stream.writeEnum(mType, &BinaryStream::writeByte);
                stream.writeString(body.mPlayerName);
                stream.writeString(body.mMessage);
            },
            [&](const TextPacket::MessageAndParams& body) {
                stream.writeEnum(mType, &BinaryStream::writeByte);
                stream.writeString(body.mMessage);
                stream.writeArray(body.mParameters, &BinaryStream::writeString);
            },
        }
    );
    stream.writeString(mXuid);
    stream.writeString(mPlatformId);
    stream.writeString(mFilteredMessage);
}

Result<> TextPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readBool(mLocalize));
    _SCULK_READ(stream.readVariant(
        mBody,
        Overload{
            [&](TextPacket::MessageOnly& body) {
                _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
                return stream.readString(body.mMessage);
            },
            [&](TextPacket::AuthorAndMessage& body) {
                _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
                _SCULK_READ(stream.readString(body.mPlayerName));
                return stream.readString(body.mMessage);
            },
            [&](TextPacket::MessageAndParams& body) {
                _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
                _SCULK_READ(stream.readString(body.mMessage));
                return stream.readArray(body.mParameters, &ReadOnlyBinaryStream::readString);
            },
        }
    ));
    _SCULK_READ(stream.readString(mXuid));
    _SCULK_READ(stream.readString(mPlatformId));
    return stream.readString(mFilteredMessage);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string TextPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mLocalize),
        SCULK_FORMAT_FIELD(mType),
        SCULK_FORMAT_FIELD(mBody),
        SCULK_FORMAT_FIELD(mXuid),
        SCULK_FORMAT_FIELD(mPlatformId),
        SCULK_FORMAT_FIELD(mFilteredMessage)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
