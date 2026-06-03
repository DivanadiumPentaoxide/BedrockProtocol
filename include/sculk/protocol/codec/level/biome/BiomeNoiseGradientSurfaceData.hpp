// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"

#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct BiomeNoiseGradientSurfaceData {
    std::vector<std::uint32_t> mNonReplaceableBlocks{};
    std::vector<std::uint32_t> mGradientBlocks{};
    std::string                mNoiseSeed{};
    int                        mFirstOctave{};
    std::vector<float>         mAmplitudes{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE