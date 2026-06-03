// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/map/MapDecoration.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void MapDecoration::write(BinaryStream& stream) const {
    stream.writeByte(mType);
    stream.writeByte(mRotation);
    stream.writeByte(mX);
    stream.writeByte(mY);
    stream.writeString(mLabel);
    stream.writeUnsignedVarInt(mColor);
}

Result<> MapDecoration::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readByte(mType));
    _SCULK_READ(stream.readByte(mRotation));
    _SCULK_READ(stream.readByte(mX));
    _SCULK_READ(stream.readByte(mY));
    _SCULK_READ(stream.readString(mLabel));
    return stream.readUnsignedVarInt(mColor);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
