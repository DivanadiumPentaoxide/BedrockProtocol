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

struct BiomeSurfaceMaterialData {
    std::uint32_t mTopBlock{};
    std::uint32_t mMidBlock{};
    std::uint32_t mSeaFloorBlock{};
    std::uint32_t mFoundationBlock{};
    std::uint32_t mSeaBlock{};
    std::int32_t  mSeaFloorDepth{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
