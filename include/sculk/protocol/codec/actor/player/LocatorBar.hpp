// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/player/UUID.hpp"
#include "sculk/protocol/codec/math/Vec2.hpp"
#include "sculk/protocol/codec/math/Vec3.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include <optional>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class LocatorBarAction : std::uint32_t {
    None   = 0,
    Add    = 1,
    Remove = 2,
    Update = 3,
};

struct LocatorBarWorldPosition {
    Vec3         mPosition{};
    std::int32_t mDimensionType{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct LocatorBarWaypointHandle {
    UUID mUUID{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct LocatorBarServerWaypointPayload {
    enum class UpdateFlags : std::uint32_t {
        Position                = 1u << 0,
        Visibility              = 1u << 1,
        TexturePath             = 1u << 2,
        IconSize                = 1u << 3,
        Color                   = 1u << 4,
        ClientPositionAuthority = 1u << 5,
        All                     = Position | Visibility | TexturePath | Color | ClientPositionAuthority | IconSize,
    };

    UpdateFlags                            mUpdateFlag{};
    std::optional<bool>                    mIsVisible{};
    std::optional<LocatorBarWorldPosition> mWorldPosition{};
    std::optional<std::string>             mTexturePath{};
    std::optional<Vec2>                    mIconSize{};
    std::optional<std::int32_t>            mColor{};
    std::optional<bool>                    mClientPositionAuthority{};
    std::optional<std::int64_t>            mActorUniqueId{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

struct LocatorBarWaypointPayload {
    LocatorBarWaypointHandle        mGroupHandle{};
    LocatorBarServerWaypointPayload mServerWaypointPayload{};
    LocatorBarAction                mActionFlag{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
