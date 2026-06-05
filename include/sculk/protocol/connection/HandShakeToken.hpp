// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/auth/PemKeyPair.hpp"
#include "sculk/protocol/utility/Result.hpp"
#include <format>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class HandShakeToken {
public:
    struct Header {
        std::string alg{};
        std::string x5u{};
    };

    struct Payload {
        std::string salt{};
    };

public:
    std::string mRawHeader{};
    Header      mHeader{};
    std::string mRawPayload{};
    Payload     mPayload{};
    std::string mSignature{};

public:
    [[nodiscard]] std::string getRemotePublicKey() const { return mHeader.x5u; }

    [[nodiscard]] std::string getSalt() const { return mPayload.salt; }

    [[nodiscard]] Result<> verify() const;

    [[nodiscard]] Result<> sign(const PemKeyPair& localKeyPair);

    [[nodiscard]] std::string toString() const { return std::format("{}.{}.{}", mRawHeader, mRawPayload, mSignature); }

    [[nodiscard]] std::vector<std::byte> getSaltBytes() const;

public:
    [[nodiscard]] static Result<HandShakeToken> fromString(std::string_view rawLoginToken);

    [[nodiscard]] static HandShakeToken fromSalt(std::span<const std::byte> salt);

    [[nodiscard]] static std::vector<std::byte> randomSalt();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE