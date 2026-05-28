// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AuthenticationKeyManager.hpp"
#include "AuthenticationType.hpp"
#include "ClientProperties.hpp"
#include "Identity.hpp"
#include "LegacyCertificateChain.hpp"
#include "LoginToken.hpp"
#include "sculk/protocol/utility/Result.hpp"
#include <optional>
#include <string>

namespace sculk::protocol::inline abi_v975 {

class ConnectionRequest {
public:
    AuthenticationType                    mAuthenticationType{};
    std::optional<LegacyCertificateChain> mLegacyCertificateChain{};
    LoginToken                            mLoginToken{};
    ClientProperties                      mClientProperties{};

public:
    [[nodiscard]] std::optional<std::string> getXboxLiveID() const;

    [[nodiscard]] std::string getXboxLiveName() const;

    [[nodiscard]] Identity getIdentity() const;

    [[nodiscard]] std::string getPlayFabID() const;

public:
    [[nodiscard]] Result<> verifyFull(const AuthenticationKeyManager& publicKeyManager) const;

    [[nodiscard]] Result<> verifySelfSigned(std::chrono::seconds leeway) const;

    [[nodiscard]] Result<> selfSign(const PemKeyPair& clientKeyPair, bool includeLegacyChain = false);

    [[nodiscard]] std::string toString() const;

public:
    [[nodiscard]] static Result<ConnectionRequest> fromString(std::string_view rawRequest);

    [[nodiscard]] static Result<ConnectionRequest> create(
        AuthenticationType                authenticationType,
        const std::optional<std::string>& legacyCertificateChainString,
        std::string_view                  loginTokenString,
        std::string_view                  clientPropertiesString
    );
};

} // namespace sculk::protocol::inline abi_v975
