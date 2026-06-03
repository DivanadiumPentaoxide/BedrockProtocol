// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "BiomeCappedSurfaceData.hpp"
#include "BiomeMesaSurfaceData.hpp"
#include "BiomeNoiseGradientSurfaceData.hpp"
#include "BiomeSurfaceMaterialData.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include <optional>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct BiomeSurfaceBuilderData {
    std::optional<BiomeSurfaceMaterialData>      mSurfaceMaterials{};
    bool                                         mHasDefaultOverworldSurface{};
    bool                                         mHasSwampSurface{};
    bool                                         mHasFrozenOceanSurface{};
    bool                                         mHasTheEndSurface{};
    std::optional<BiomeMesaSurfaceData>          mMesaSurface{};
    std::optional<BiomeCappedSurfaceData>        mCappedSurface{};
    std::optional<BiomeNoiseGradientSurfaceData> mNoiseGradientSurface{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE