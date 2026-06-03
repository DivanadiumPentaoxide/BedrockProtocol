// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "BiomeCoordinateData.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct BiomeScatterParamData {
    enum class CoordinateEvaluationOrder : std::int32_t {
        XYZ = 0,
        XZY = 1,
        YXZ = 2,
        YZX = 3,
        ZXY = 4,
        ZYX = 5,
    };

    std::vector<BiomeCoordinateData> mCoordinate{};
    CoordinateEvaluationOrder        mEvalOrder{};
    std::int32_t                     mChancePercentType{};
    std::uint16_t                    mChancePercent{};
    std::int32_t                     mChanceNumeartor{};
    std::int32_t                     mChanceDenominator{};
    std::int32_t                     mIterationsType{};
    std::uint16_t                    mIterations{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
