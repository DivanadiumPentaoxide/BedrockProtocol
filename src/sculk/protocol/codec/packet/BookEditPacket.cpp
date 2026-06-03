// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/BookEditPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds BookEditPacket::getId() const noexcept { return MinecraftPacketIds::BookEdit; }

std::string_view BookEditPacket::getName() const noexcept { return "BookEditPacket"; }

void BookEditPacket::write(BinaryStream& stream) const {
    stream.writeVarInt(mBookSlot);
    stream.writeEnum(mAction, &BinaryStream::writeUnsignedVarInt);
    switch (mAction) {
    case Action::ReplacePage:
    case Action::AddPage:
        stream.writeVarInt(mPageIndexA);
        stream.writeString(mTextA);
        stream.writeString(mTextB);
        break;
    case Action::DeletePage:
        stream.writeVarInt(mPageIndexA);
        break;
    case Action::SwapPages:
        stream.writeVarInt(mPageIndexA);
        stream.writeVarInt(mPageIndexB);
        break;
    case Action::Finalize:
        stream.writeString(mTextA);
        stream.writeString(mTextB);
        stream.writeString(mXuid);
        break;
    default:
        break;
    }
}

Result<> BookEditPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mBookSlot));
    _SCULK_READ(stream.readEnum(mAction, &ReadOnlyBinaryStream::readUnsignedVarInt));
    switch (mAction) {
    case Action::ReplacePage:
    case Action::AddPage:
        _SCULK_READ(stream.readVarInt(mPageIndexA));
        _SCULK_READ(stream.readString(mTextA));
        return stream.readString(mTextB);
    case Action::DeletePage:
        return stream.readVarInt(mPageIndexA);
    case Action::SwapPages:
        _SCULK_READ(stream.readVarInt(mPageIndexA));
        return stream.readVarInt(mPageIndexB);
    case Action::Finalize:
        _SCULK_READ(stream.readString(mTextA));
        _SCULK_READ(stream.readString(mTextB));
        return stream.readString(mXuid);
    default:
        return {};
    }
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string BookEditPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mAction),
        SCULK_FORMAT_FIELD(mBookSlot),
        SCULK_FORMAT_FIELD(mPageIndexA),
        SCULK_FORMAT_FIELD(mPageIndexB),
        SCULK_FORMAT_FIELD(mTextA),
        SCULK_FORMAT_FIELD(mTextB),
        SCULK_FORMAT_FIELD(mXuid)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
