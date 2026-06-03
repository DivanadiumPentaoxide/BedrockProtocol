// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/level/StructureSettings.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct StructureEditorData {
    std::string       mStructureName{};
    std::string       mFilteredStructureName{};
    std::string       mDataField{};
    bool              mIncludePlayer{false};
    bool              mShowBoundingBox{false};
    std::int32_t      mStructureBlockType{};
    StructureSettings mStructureSettings{};
    std::int32_t      mRedstoneSaveMode{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
