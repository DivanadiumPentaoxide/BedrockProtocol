// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/LoginPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds LoginPacket::getId() const noexcept { return MinecraftPacketIds::Login; }

std::string_view LoginPacket::getName() const noexcept { return "LoginPacket"; }

void LoginPacket::write(BinaryStream& stream) const {
    stream.writeSignedBigEndianInt(mNetworkVersion);
    stream.writeString(mRawConnectionRequest);
}

Result<> LoginPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedBigEndianInt(mNetworkVersion));
    return stream.readString(mRawConnectionRequest);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string LoginPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mNetworkVersion), SCULK_FORMAT_FIELD(mRawConnectionRequest));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
