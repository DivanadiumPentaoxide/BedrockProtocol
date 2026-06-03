// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeLegacyWorldGenRulesData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeLegacyWorldGenRulesData::write(BinaryStream& stream) const {
    stream.writeArray(mLegacyPreHills, &BiomeConditionalTransformationData::write);
}

Result<> BiomeLegacyWorldGenRulesData::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mLegacyPreHills, &BiomeConditionalTransformationData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
