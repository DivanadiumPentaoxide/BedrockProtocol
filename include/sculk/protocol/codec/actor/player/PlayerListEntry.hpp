// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/player/SerializedSkin.hpp"
#include "sculk/protocol/codec/actor/player/UUID.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct PlayerListEntry {
    UUID           mUUID{};
    std::int64_t   mActorUniqueId{};
    std::string    mPlayerName{};
    std::string    mXuid{};
    std::string    mPlatformChatId{};
    std::int32_t   mBuildPlatform{};
    SerializedSkin mSerializedSkin{};
    bool           mIsTeacher{};
    bool           mIsHost{};
    bool           mIsSubClient{};
    std::int32_t   mColor{};
    bool           mSkinTrusted{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
