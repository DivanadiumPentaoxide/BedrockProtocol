// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/item/NetworkItemInstanceDescriptor.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void NetworkItemInstanceDescriptor::write(BinaryStream& stream) const {
    if (mId == 0) {
        stream.writeVarInt(0);
    } else {
        stream.writeVarInt(mId);
        stream.writeUnsignedShort(mStackSize);
        stream.writeUnsignedVarInt(mAux);
        stream.writeVarInt(mBlockRuntimeId);
        stream.writeString(mUserData);
    }
}

Result<> NetworkItemInstanceDescriptor::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mId));
    if (mId == 0) {
        return {};
    } else {
        _SCULK_READ(stream.readUnsignedShort(mStackSize));
        _SCULK_READ(stream.readUnsignedVarInt(mAux));
        _SCULK_READ(stream.readVarInt(mBlockRuntimeId));
        return stream.readString(mUserData);
    }
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
