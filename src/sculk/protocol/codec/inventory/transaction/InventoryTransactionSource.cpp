// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/transaction/InventoryTransactionSource.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void InventoryTransactionSource::write(BinaryStream& stream) const {
    stream.writeEnum(mType, &BinaryStream::writeUnsignedVarInt);
    switch (mType) {
    case InventoryTransactionSourceType::ContainerInventory:
    case InventoryTransactionSourceType::NonImplementedFeatureTODO:
        stream.writeVarInt(mContainerId);
        break;
    case InventoryTransactionSourceType::WorldInteraction:
        stream.writeUnsignedVarInt(mBitFlags);
        break;
    default:
        break;
    }
}

Result<> InventoryTransactionSource::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readUnsignedVarInt));
    switch (mType) {
    case InventoryTransactionSourceType::ContainerInventory:
    case InventoryTransactionSourceType::NonImplementedFeatureTODO:
        return stream.readVarInt(mContainerId);
    case InventoryTransactionSourceType::WorldInteraction:
        return stream.readUnsignedVarInt(mBitFlags);
    default:
        return {};
    }
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE