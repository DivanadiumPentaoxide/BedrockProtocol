// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/player/PlayerActionType.hpp"
#include "sculk/protocol/codec/level/block/BlockPos.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct PlayerBlockActionData {
    PlayerActionType mActionType{};
    BlockPos         mPosition{};
    std::int32_t     mFacing{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct PlayerBlockActions {
    std::vector<PlayerBlockActionData> mActions{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
