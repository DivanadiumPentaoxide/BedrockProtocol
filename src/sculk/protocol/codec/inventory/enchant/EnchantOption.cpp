// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/enchant/EnchantOption.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void Enchant::write(BinaryStream& stream) const {
    stream.writeEnum(mType, &BinaryStream::writeByte);
    stream.writeByte(mLevel);
}

Result<> Enchant::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
    return stream.readByte(mLevel);
}

void ItemEnchants::write(BinaryStream& stream) const {
    stream.writeSignedInt(mSlot);
    stream.writeArray(mOption1, &Enchant::write);
    stream.writeArray(mOption2, &Enchant::write);
    stream.writeArray(mOption3, &Enchant::write);
}

Result<> ItemEnchants::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedInt(mSlot));
    _SCULK_READ(stream.readArray(mOption1, &Enchant::read));
    _SCULK_READ(stream.readArray(mOption2, &Enchant::read));
    return stream.readArray(mOption3, &Enchant::read);
}

void EnchantOption::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt(mCost);
    mEnchants.write(stream);
    stream.writeString(mEnchantName);
    stream.writeUnsignedVarInt(mNetId);
}

Result<> EnchantOption::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt(mCost));
    _SCULK_READ(mEnchants.read(stream));
    _SCULK_READ(stream.readString(mEnchantName));
    return stream.readUnsignedVarInt(mNetId);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
