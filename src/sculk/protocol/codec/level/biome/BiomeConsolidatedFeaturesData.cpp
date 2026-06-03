// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/biome/BiomeConsolidatedFeaturesData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BiomeConsolidatedFeaturesData::write(BinaryStream& stream) const {
    stream.writeArray(mFeatures, &BiomeConsolidatedFeatureData::write);
}

Result<> BiomeConsolidatedFeaturesData::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mFeatures, &BiomeConsolidatedFeatureData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
