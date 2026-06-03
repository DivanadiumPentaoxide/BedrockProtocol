// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/Result.hpp"
#include <string>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct PemKeyPair {
    std::string mPublicKeyPem{};
    std::string mPrivateKeyPem{};
};

namespace ssl {

[[nodiscard]] Result<PemKeyPair> randomES384KeyPair();

[[nodiscard]] Result<PemKeyPair> randomRS256KeyPair();

} // namespace ssl

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE