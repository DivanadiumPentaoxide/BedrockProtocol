// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/inventory/container/FullContainerName.hpp"
#include <string>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class ItemStackNetResult : std::uint8_t {
    Success = 0,
    Error   = 1,
};

struct ItemStackResponseSlotInfo {
    std::uint8_t mRequestedSlot{};
    std::uint8_t mSlot{};
    std::uint8_t mAmount{};
    std::int32_t mNetId{};
    std::string  mCustomName{};
    std::string  mFilteredCustomName{};
    std::int32_t mDurationCorrection{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct ItemStackResponseContainerInfo {
    FullContainerName                      mContainerName{};
    std::vector<ItemStackResponseSlotInfo> mSlots{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct ItemStackResponseInfo {
    ItemStackNetResult                          mResult{};
    std::int32_t                                mRequestId{};
    std::vector<ItemStackResponseContainerInfo> mContainers{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct ItemStackResponse {
    std::vector<ItemStackResponseInfo> mResponses{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
