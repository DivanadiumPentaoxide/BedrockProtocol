// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/command/CommandOverloadData.hpp"
#include "sculk/protocol/codec/command/CommandPermissionLevel.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct CommandData {
    std::string                      mName{};
    std::string                      mDescription{};
    std::uint16_t                    mFlags{};
    CommandPermissionLevel           mCommandPermissionLevel{};
    std::int32_t                     mAliasEnum{};
    std::vector<std::uint32_t>       mChainedSubcommandIndexes{};
    std::vector<CommandOverloadData> mOverloads{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
