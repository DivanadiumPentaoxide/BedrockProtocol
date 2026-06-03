// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/SpawnSettings.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void SpawnSettings::write(BinaryStream& stream) const {
    stream.writeSignedShort(mType);
    stream.writeString(mBiomeName);
    stream.writeVarInt(mDimension);
}

Result<> SpawnSettings::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedShort(mType));
    _SCULK_READ(stream.readString(mBiomeName));
    return stream.readVarInt(mDimension);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
