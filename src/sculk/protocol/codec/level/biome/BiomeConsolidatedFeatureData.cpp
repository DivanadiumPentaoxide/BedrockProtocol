// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeConsolidatedFeatureData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeConsolidatedFeatureData::write(BinaryStream& stream) const {
    mScatter.write(stream);
    stream.writeUnsignedShort(mFeature);
    stream.writeUnsignedShort(mIdentifier);
    stream.writeUnsignedShort(mPass);
    stream.writeBool(mCanUseInternal);
}

Result<> BiomeConsolidatedFeatureData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mScatter.read(stream));
    _SCULK_READ(stream.readUnsignedShort(mFeature));
    _SCULK_READ(stream.readUnsignedShort(mIdentifier));
    _SCULK_READ(stream.readUnsignedShort(mPass));
    return stream.readBool(mCanUseInternal);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
