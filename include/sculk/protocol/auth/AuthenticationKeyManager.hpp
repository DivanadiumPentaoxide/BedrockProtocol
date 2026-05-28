// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AuthenticationType.hpp"
#include "PemKeyPair.hpp"
#include <chrono>
#include <future>
#include <string>
#include <unordered_map>

namespace sculk::protocol::inline abi_v975 {

class AuthenticationKeyManager {
    std::unordered_map<std::string, std::string> mPublicKeysPemByKeyId{};
    std::string                                  mExpectedIssuer{};
    std::string                                  mExpectedPlayFabTitle{};
    std::chrono::seconds                         mLeeway{60};

public:
    AuthenticationKeyManager() = default;

    [[nodiscard]] std::optional<std::string_view> getPublicKeyPemByKeyId(const std::string& keyId) const;

    [[nodiscard]] std::string_view getExpectedIssuer() const;

    [[nodiscard]] std::string_view getExpectedPlayFabTitle() const;

    [[nodiscard]] std::chrono::seconds getLeeway() const { return mLeeway; }

public:
    constexpr AuthenticationKeyManager& setLeeway(std::chrono::seconds leeway) {
        mLeeway = leeway;
        return *this;
    }

    Result<> initMojangPublicKeyBlocking(std::size_t timeoutSeconds);

    std::future<Result<>> initMojangPublicKeyAsync(std::size_t timeoutSeconds);
};

} // namespace sculk::protocol::inline abi_v975