// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include <cstdint>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

template <typename T>
struct ValueArrayTag {
    std::vector<T> mValue{};

    void serialize(BinaryStream& stream) const {
        stream.writeVarInt(static_cast<std::int32_t>(mValue.size()));
        for (const T& val : mValue) {
            if constexpr (std::is_same_v<T, std::int8_t>) {
                stream.writeSignedChar(val);
            } else if constexpr (std::is_same_v<T, std::int32_t>) {
                stream.writeVarInt(val);
            } else {
                static_assert(false, "Invalid ValueTag<T> template");
            }
        }
    }

    [[nodiscard]] Result<> deserialize(ReadOnlyBinaryStream& stream) {
        std::int32_t size{};
        _SCULK_READ(stream.readVarInt(size));
        mValue.resize(size);
        for (std::int32_t i = 0; i < size; ++i) {
            if constexpr (std::is_same_v<T, std::int8_t>) {
                _SCULK_READ(stream.readSignedChar(mValue[i]));
            } else if constexpr (std::is_same_v<T, std::int32_t>) {
                _SCULK_READ(stream.readVarInt(mValue[i]));
            } else {
                static_assert(false, "Invalid ValueTag<T> template");
            }
        }
        return Result<>{};
    }
};

using ByteArrayTag = ValueArrayTag<std::int8_t>;
using IntArrayTag  = ValueArrayTag<std::int32_t>;

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
