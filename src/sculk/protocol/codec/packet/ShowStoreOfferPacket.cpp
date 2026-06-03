// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ShowStoreOfferPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ShowStoreOfferPacket::getId() const noexcept { return MinecraftPacketIds::ShowStoreOffer; }

std::string_view ShowStoreOfferPacket::getName() const noexcept { return "ShowStoreOfferPacket"; }

void ShowStoreOfferPacket::write(BinaryStream& stream) const {
    stream.writeString(mProductUuid);
    stream.writeEnum(mRedirectType, &BinaryStream::writeByte);
}

Result<> ShowStoreOfferPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mProductUuid));
    return stream.readEnum(mRedirectType, &ReadOnlyBinaryStream::readByte);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ShowStoreOfferPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mProductUuid), SCULK_FORMAT_FIELD(mRedirectType));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
