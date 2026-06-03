// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/LecternUpdatePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds LecternUpdatePacket::getId() const noexcept { return MinecraftPacketIds::LecternUpdate; }

std::string_view LecternUpdatePacket::getName() const noexcept { return "LecternUpdatePacket"; }

void LecternUpdatePacket::write(BinaryStream& stream) const {
    stream.writeByte(mNewPageToShow);
    stream.writeByte(mTotalPages);
    mPosition.write(stream);
}

Result<> LecternUpdatePacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readByte(mNewPageToShow));
    _SCULK_READ(stream.readByte(mTotalPages));
    return mPosition.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LecternUpdatePacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mNewPageToShow),
        SCULK_FORMAT_FIELD(mTotalPages),
        SCULK_FORMAT_FIELD(mPosition)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
