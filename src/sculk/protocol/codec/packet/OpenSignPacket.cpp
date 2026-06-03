// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/OpenSignPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds OpenSignPacket::getId() const noexcept { return MinecraftPacketIds::OpenSign; }

std::string_view OpenSignPacket::getName() const noexcept { return "OpenSignPacket"; }

void OpenSignPacket::write(BinaryStream& stream) const {
    mPosition.write(stream);
    stream.writeBool(mIsFrontSide);
}

Result<> OpenSignPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPosition.read(stream));
    return stream.readBool(mIsFrontSide);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string OpenSignPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mPosition), SCULK_FORMAT_FIELD(mIsFrontSide));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
