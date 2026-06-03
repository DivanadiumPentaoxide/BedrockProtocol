// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/item/ItemData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ItemData::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeSignedShort(mId);
    stream.writeBool(mIsComponentBased);
    stream.writeEnum(mVersion, &BinaryStream::writeVarInt);
    mComponentData.write(stream);
}

Result<> ItemData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readSignedShort(mId));
    _SCULK_READ(stream.readBool(mIsComponentBased));
    _SCULK_READ(stream.readEnum(mVersion, &ReadOnlyBinaryStream::readVarInt));
    return mComponentData.read(stream);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
