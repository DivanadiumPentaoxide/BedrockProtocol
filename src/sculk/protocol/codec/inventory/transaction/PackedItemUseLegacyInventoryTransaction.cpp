// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/transaction/PackedItemUseLegacyInventoryTransaction.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void PackedItemUseLegacyInventoryTransaction::write(BinaryStream& stream) const {
    stream.writeVarInt(mLegacyRequestRawId);
    if (mLegacyRequestRawId < -1 && (mLegacyRequestRawId & 1) == 0) {
        stream.writeArray(mLegacySetItemSlots, &LegacySetItemSlot::write);
    }
    mTransactionActions.write(stream);
    mItemUseTransaction.write(stream);
}

Result<> PackedItemUseLegacyInventoryTransaction::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mLegacyRequestRawId));
    if (mLegacyRequestRawId < -1 && (mLegacyRequestRawId & 1) == 0) {
        _SCULK_READ(stream.readArray(mLegacySetItemSlots, &LegacySetItemSlot::read));
    }
    _SCULK_READ(mTransactionActions.read(stream));
    return mItemUseTransaction.read(stream);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
