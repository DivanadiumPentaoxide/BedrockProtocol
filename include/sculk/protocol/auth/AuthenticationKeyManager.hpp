// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "PemKeyPair.hpp"
#include <chrono>
#include <future>
#include <parallel_hashmap/phmap.h>
#include <string>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class AuthenticationKeyManager {
    phmap::flat_hash_map<std::string, std::string> mPublicKeysPemByKeyId{};
    std::string                                    mExpectedIssuer{};
    std::string                                    mExpectedPlayFabTitle{};
    std::chrono::seconds                           mLeeway{};

public:
    AuthenticationKeyManager(std::chrono::seconds leeway = std::chrono::seconds(60));

    [[nodiscard]] std::optional<std::string_view> getPublicKeyPemByKeyId(const std::string& keyId) const;

    [[nodiscard]] std::string_view getExpectedIssuer() const;

    [[nodiscard]] std::string_view getExpectedPlayFabTitle() const;

    [[nodiscard]] std::chrono::seconds getLeeway() const { return mLeeway; }

public:
    AuthenticationKeyManager& setLeeway(std::chrono::seconds leeway);

    Result<> initMojangPublicKeyFromNetwork(std::size_t timeoutSeconds = 5);

    void initMojangPublicKeyFromCachedKeys();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE