// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/TrimDataPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void TrimDataPacket::TrimPattern::write(BinaryStream& stream) const {
    stream.writeString(mItemName);
    stream.writeString(mPatternId);
}

Result<> TrimDataPacket::TrimPattern::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mItemName));
    return stream.readString(mPatternId);
}

void TrimDataPacket::TrimMaterial::write(BinaryStream& stream) const {
    stream.writeString(mMaterialId);
    stream.writeString(mColor);
    stream.writeString(mItemName);
}

Result<> TrimDataPacket::TrimMaterial::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mMaterialId));
    _SCULK_READ(stream.readString(mColor));
    return stream.readString(mItemName);
}

MinecraftPacketIds TrimDataPacket::getId() const noexcept { return MinecraftPacketIds::TrimData; }
std::string_view   TrimDataPacket::getName() const noexcept { return "TrimDataPacket"; }
void               TrimDataPacket::write(BinaryStream& stream) const {
    stream.writeArray(mTrimPatternList, &TrimPattern::write);
    stream.writeArray(mTrimMaterialList, &TrimMaterial::write);
}
Result<> TrimDataPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mTrimPatternList, &TrimPattern::read));
    return stream.readArray(mTrimMaterialList, &TrimMaterial::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string TrimDataPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mTrimPatternList), SCULK_FORMAT_FIELD(mTrimMaterialList));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
