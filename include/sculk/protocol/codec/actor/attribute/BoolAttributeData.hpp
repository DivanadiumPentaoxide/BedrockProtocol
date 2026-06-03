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

enum class BoolAttributeOperation : std::int32_t {
    OVERRIDE    = 0,
    ALPHA_BLEND = 1,
    AND         = 2,
    NAND        = 3,
    OR          = 4,
    NOR         = 5,
    XOR         = 6,
    XNOR        = 7,
};

struct BoolAttributeData {
    bool                   mValue{};
    BoolAttributeOperation mOperation{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
