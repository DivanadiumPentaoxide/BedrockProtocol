// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/nbt/TagVariant.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct ItemData {
    enum class ItemVersion : std::int32_t {
        Legacy     = 0,
        DataDriven = 1,
        None       = 2,
    };

    std::string  mName{};
    std::int16_t mId{};
    bool         mIsComponentBased{false};
    ItemVersion  mVersion{};
    CompoundTag  mComponentData{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
