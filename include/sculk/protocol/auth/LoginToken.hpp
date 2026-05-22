// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/Result.hpp"
#include <optional>
#include <string>

namespace sculk::protocol::inline abi_v975 {

class LoginToken {
public:
    struct Header {
        std::string                alg{};
        std::optional<std::string> kid{};
        std::optional<std::string> x5u{};
        std::optional<std::string> typ{};
    };

public:
    [[nodiscard]] std::string toString() const;

public:
    [[nodiscard]] static Result<LoginToken> fromString(std::string_view rawLoginToken);
};

} // namespace sculk::protocol::inline abi_v975