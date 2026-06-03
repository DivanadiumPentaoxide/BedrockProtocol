// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/chunk/SubChunkBlockChangeInfo.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void SubChunkBlockChangeInfo::write(BinaryStream& stream) const {
    mPosition.write(stream);
    stream.writeUnsignedVarInt(mRuntimeId);
    stream.writeUnsignedVarInt(mFlags);
    stream.writeUnsignedVarInt64(mSyncEntity);
    stream.writeUnsignedVarInt(mSyncMessage);
}

Result<> SubChunkBlockChangeInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(stream.readUnsignedVarInt(mRuntimeId));
    _SCULK_READ(stream.readUnsignedVarInt(mFlags));
    _SCULK_READ(stream.readUnsignedVarInt64(mSyncEntity));
    return stream.readUnsignedVarInt(mSyncMessage);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
