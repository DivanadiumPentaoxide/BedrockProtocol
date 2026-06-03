// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/Result.hpp"
#include <cstddef>
#include <span>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::compression::zlib {

[[nodiscard]] std::vector<std::byte> compress(std::span<const std::byte> data);

[[nodiscard]] Result<std::vector<std::byte>> decompress(std::span<const std::byte> data);

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::compression::zlib