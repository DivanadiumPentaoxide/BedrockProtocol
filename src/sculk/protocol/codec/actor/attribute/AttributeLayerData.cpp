// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/AttributeLayerData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void AttributeLayerData::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeVarInt(mDimension);
    mSettings.write(stream);
    stream.writeArray(mAttributes, &EnvironmentAttributeData::write);
}

Result<> AttributeLayerData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readVarInt(mDimension));
    _SCULK_READ(mSettings.read(stream));
    return stream.readArray(mAttributes, &EnvironmentAttributeData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
