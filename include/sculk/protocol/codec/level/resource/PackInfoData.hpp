// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/player/UUID.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct PackInfoData {
    UUID          mPackId{};
    std::string   mPackVersion{};
    std::uint64_t mPackSize{};
    std::string   mContentKey{};
    std::string   mSubpackName{};
    std::string   mContentIdentity{};
    bool          mHasScripts{};
    bool          mIsAddonPack{};
    bool          mIsRayTracingCapable{};
    std::string   mCDNUrl{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
