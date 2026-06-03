// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/transaction/ItemUseOnActorInventoryTransaction.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ItemUseOnActorInventoryTransaction::write(BinaryStream& stream) const {
    stream.writeUnsignedVarInt64(mRuntimeId);
    stream.writeEnum(mActionType, &BinaryStream::writeUnsignedVarInt);
    stream.writeVarInt(mSlot);
    mItem.write(stream);
    mFromPos.write(stream);
    mHitPos.write(stream);
}

Result<> ItemUseOnActorInventoryTransaction::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedVarInt64(mRuntimeId));
    _SCULK_READ(stream.readEnum(mActionType, &ReadOnlyBinaryStream::readUnsignedVarInt));
    _SCULK_READ(stream.readVarInt(mSlot));
    _SCULK_READ(mItem.read(stream));
    _SCULK_READ(mFromPos.read(stream));
    return mHitPos.read(stream);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
