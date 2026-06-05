// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <string_view>

#define SCULK_NETWORK_PROTOCOL_VERSION 975
#define SCULK_MINECRAFT_VERSION        "26.20"

#define SCULK_ABI_INLINE_NAMESPACE inline abi_v975

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

constexpr int getProtocolVersion() noexcept { return SCULK_NETWORK_PROTOCOL_VERSION; }

constexpr std::string_view getMinecraftVersion() noexcept { return SCULK_MINECRAFT_VERSION; }

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE