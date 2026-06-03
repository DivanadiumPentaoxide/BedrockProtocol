// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/player/UUID.hpp"
#include "sculk/protocol/codec/command/CommandOriginType.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct CommandOriginData {
    CommandOriginType mType{};
    UUID              mUuid{};
    std::string       mRequestId{};
    std::int64_t      mPlayerId{-1};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
