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

enum class FloatAttributeOperation : std::int32_t {
    OVERRIDE    = 0,
    ALPHA_BLEND = 1,
    ADD         = 2,
    SUBTRACT    = 3,
    MULTIPLY    = 4,
    MINIMUM     = 5,
    MAXIMUM     = 6,
};

struct FloatAttributeData {
    float                   mValue{};
    FloatAttributeOperation mOperation{};
    float                   mConstraintMin{};
    float                   mConstraintMax{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
