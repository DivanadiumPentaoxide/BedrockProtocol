// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/map/MapTrackedActorUniqueId.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void MapTrackedActorUniqueId::write(BinaryStream& stream) const {
    stream.writeEnum(mType, &BinaryStream::writeSignedInt);
    switch (mType) {
    case Type::Entity:
        stream.writeVarInt64(mActorUniqueId);
        break;
    case Type::BlockEntity:
        mBlockPosition.write(stream);
        break;
    default:
        break;
    }
}

Result<> MapTrackedActorUniqueId::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readSignedInt));
    switch (mType) {
    case Type::Entity:
        return stream.readVarInt64(mActorUniqueId);
    case Type::BlockEntity:
        return mBlockPosition.read(stream);
    default:
        return {};
    }
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
