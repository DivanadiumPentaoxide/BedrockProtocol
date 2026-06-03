// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/transaction/ItemUseInventoryTransaction.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ItemUseInventoryTransaction::write(BinaryStream& stream) const {
    stream.writeEnum(mActionType, &BinaryStream::writeUnsignedVarInt);
    stream.writeEnum(mTriggerType, &BinaryStream::writeUnsignedVarInt);
    mPos.write(stream);
    stream.writeVarInt(mFace);
    stream.writeVarInt(mSlot);
    mItem.write(stream);
    mFromPos.write(stream);
    mClickPos.write(stream);
    stream.writeUnsignedVarInt(mTargetBlockId);
    stream.writeEnum(mClientPredictedResult, &BinaryStream::writeByte);
    stream.writeEnum(mClientCooldownState, &BinaryStream::writeByte);
}

Result<> ItemUseInventoryTransaction::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mActionType, &ReadOnlyBinaryStream::readUnsignedVarInt));
    _SCULK_READ(stream.readEnum(mTriggerType, &ReadOnlyBinaryStream::readUnsignedVarInt));
    _SCULK_READ(mPos.read(stream));
    _SCULK_READ(stream.readVarInt(mFace));
    _SCULK_READ(stream.readVarInt(mSlot));
    _SCULK_READ(mItem.read(stream));
    _SCULK_READ(mFromPos.read(stream));
    _SCULK_READ(mClickPos.read(stream));
    _SCULK_READ(stream.readUnsignedVarInt(mTargetBlockId));
    _SCULK_READ(stream.readEnum(mClientPredictedResult, &ReadOnlyBinaryStream::readByte));
    return stream.readEnum(mClientCooldownState, &ReadOnlyBinaryStream::readByte);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
