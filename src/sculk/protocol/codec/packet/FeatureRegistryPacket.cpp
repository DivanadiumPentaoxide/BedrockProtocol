// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/FeatureRegistryPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void FeatureRegistryPacket::FeatureData::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeString(mBinaryJsonOutput);
}

Result<> FeatureRegistryPacket::FeatureData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    return stream.readString(mBinaryJsonOutput);
}

MinecraftPacketIds FeatureRegistryPacket::getId() const noexcept { return MinecraftPacketIds::FeatureRegistry; }

std::string_view FeatureRegistryPacket::getName() const noexcept { return "FeatureRegistryPacket"; }

void FeatureRegistryPacket::write(BinaryStream& stream) const {
    stream.writeArray(mFuturesDataList, &FeatureData::write);
}

Result<> FeatureRegistryPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mFuturesDataList, &FeatureData::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string FeatureRegistryPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mFuturesDataList));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
