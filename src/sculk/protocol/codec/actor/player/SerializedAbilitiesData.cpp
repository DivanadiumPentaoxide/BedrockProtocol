// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/player/SerializedAbilitiesData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void SerializedAbilitiesData::write(BinaryStream& stream) const {
    stream.writeSignedInt64(mPlayerRawId);
    stream.writeByte(mPlayerPermission);
    stream.writeByte(mCommandPermission);
    stream.writeArray(mLayers, [](BinaryStream& stream, const SerializedLayer& layer) {
        stream.writeUnsignedShort(layer.mLayer);
        stream.writeUnsignedInt(layer.mAbilitiesSet);
        stream.writeUnsignedInt(layer.mAbilitiesValue);
        stream.writeFloat(layer.mFlySpeed);
        stream.writeFloat(layer.mVerticalFlySpeed);
        stream.writeFloat(layer.mWalkSpeed);
    });
}

Result<> SerializedAbilitiesData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedInt64(mPlayerRawId));
    _SCULK_READ(stream.readByte(mPlayerPermission));
    _SCULK_READ(stream.readByte(mCommandPermission));
    _SCULK_READ(stream.readArray(mLayers, [](ReadOnlyBinaryStream& stream, SerializedLayer& layer) {
        _SCULK_READ(stream.readUnsignedShort(layer.mLayer));
        _SCULK_READ(stream.readUnsignedInt(layer.mAbilitiesSet));
        _SCULK_READ(stream.readUnsignedInt(layer.mAbilitiesValue));
        _SCULK_READ(stream.readFloat(layer.mFlySpeed));
        _SCULK_READ(stream.readFloat(layer.mVerticalFlySpeed));
        return stream.readFloat(layer.mWalkSpeed);
    }));
    return {};
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
