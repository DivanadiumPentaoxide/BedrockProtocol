// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct BiomeCoordinateData {
    enum class RandomDistributionType : std::int32_t {
        SingleValued    = 0,
        Uniform         = 1,
        Gaussian        = 2,
        InverseGaussian = 3,
        FixedGrid       = 4,
        JitteredGrid    = 5,
        Triangle        = 6
    };

    std::int32_t           mMinValueType{};
    std::uint16_t          mMinValue{};
    std::int32_t           mMaxValueType{};
    std::uint16_t          mMaxValue{};
    std::uint32_t          mGridOffset{};
    std::uint32_t          mGridStepSize{};
    RandomDistributionType mDistributionType{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
