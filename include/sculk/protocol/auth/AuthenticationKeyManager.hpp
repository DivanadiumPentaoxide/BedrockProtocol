// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AuthenticationType.hpp"
#include "sculk/protocol/utility/Result.hpp"
#include <chrono>
#include <future>
#include <optional>
#include <string>
#include <unordered_map>

namespace sculk::protocol::inline abi_v975 {

class AuthenticationKeyManager {
public:
    struct KeyPair {
        std::string mPublicKeyPem{};
        std::string mPrivateKeyPem{};
    };

private:
    AuthenticationType                                   mVerifyAuthenticationType{};
    AuthenticationType                                   mSigningAuthenticationType{};
    std::unordered_map<std::string, std::string>         mLoginTokenPublicKeysPemByKeyId{};
    std::optional<std::pair<std::string, KeyPair>>       mLoginTokenKeyPairsAndKeyId{};
    std::string                                          mLoginTokenExpectedIssuer{};
    std::string                                          mLoginTokenExpectedPlayFabTitle{};
    std::optional<KeyPair>                               mSelfSignedLoginTokenKeyPair{};
    std::vector<std::string>                             mLegacyCertificateChainPublicKeyPems{};
    std::optional<KeyPair>                               mLegacyCertificateClientKeyPair{};
    std::optional<KeyPair>                               mLegacyCertificateMojangKeyPair{};
    std::optional<KeyPair>                               mLegacyCertificateLoginKeyPair{};
    std::chrono::seconds                                 mValidityLeeway{60};
    std::optional<std::chrono::system_clock::time_point> mValidityTime{};
    std::optional<std::chrono::system_clock::time_point> mSigningTime{};

public:
    AuthenticationKeyManager() = default;

    [[nodiscard]] constexpr AuthenticationType getVerifyAuthenticationType() const { return mVerifyAuthenticationType; }

    [[nodiscard]] constexpr std::chrono::seconds getValidityLeeway() const { return mValidityLeeway; }

    [[nodiscard]] std::chrono::system_clock::time_point getValidityTime() const;

    [[nodiscard]] const std::vector<std::string>& getLegacyCertificateChainPublicKeyPems() const;

    [[nodiscard]] std::optional<std::string_view> getLoginTokenPublicKeyPemByKeyId(const std::string& keyId) const;

    [[nodiscard]] std::string_view getLoginTokenExpectedIssuer() const;

    [[nodiscard]] std::string_view getLoginTokenExpectedPlayFabTitle() const;

public:
    [[nodiscard]] Result<KeyPair> generateRandomES384KeyPair() const;

    [[nodiscard]] Result<KeyPair> generateRandomRS256KeyPair() const;

public:
    [[nodiscard]] constexpr AuthenticationType getSigningAuthenticationType() const {
        return mSigningAuthenticationType;
    }

    [[nodiscard]] std::chrono::system_clock::time_point getSigningTime() const;

    [[nodiscard]] bool legacyCertificateChainSigningInitialized(AuthenticationType authType) const;

    [[nodiscard]] Result<> generateAndSetLegacyFullCertificateChainKeyPairs();

    [[nodiscard]] Result<> generateAndSetLegacySelfSignedCertificateChainKeyPairs();

    void setLegacyCertificateChainClientKeyPair(std::string_view publicKeyPem, std::string_view privateKeyPem);

    void setLegacyCertificateChainMojangKeyPair(std::string_view publicKeyPem, std::string_view privateKeyPem);

    void setLegacyCertificateChainLoginKeyPair(std::string_view publicKeyPem, std::string_view privateKeyPem);

    [[nodiscard]] Result<KeyPair> getLegacyCertificateChainClientKeyPair() const;

    [[nodiscard]] Result<KeyPair> getLegacyCertificateChainMojangKeyPair() const;

    [[nodiscard]] Result<KeyPair> getLegacyCertificateChainLoginKeyPair() const;

public:
    [[nodiscard]] bool loginTokenSigningInitialized(AuthenticationType authType) const;

    [[nodiscard]] std::string generateRandomKeyId() const;

    [[nodiscard]] Result<> generateAndSetLoginTokenKeyPairFull();

    [[nodiscard]] Result<> generateAndSetLoginTokenKeyPairSelfSigned();

    void
    setLoginTokenKeyPairFull(const std::string& keyId, std::string_view publicKeyPem, std::string_view privateKeyPem);

    void setLoginTokenKeyPairSelfSigned(std::string_view publicKeyPem, std::string_view privateKeyPem);

    [[nodiscard]] Result<KeyPair> getFullLoginTokenKeyPairAndKeyId(std::string& outKeyId) const;

    [[nodiscard]] Result<KeyPair> getSelfSignedLoginTokenKeyPair() const;

public:
    void addLoginTokenPublicKeyPemByKeyId(const std::string& keyId, const std::string& publicKeyPem);

    void addLegacyCertificateChainPublicKeyPem(std::string_view publicKeyPem);

public:
    [[nodiscard]] Result<KeyPair> getClientPropertiesKeyPair() const;

public:
    constexpr void setVerifyAuthenticationType(AuthenticationType authType) { mVerifyAuthenticationType = authType; }

    constexpr void setSigningAuthenticationType(AuthenticationType authType) { mSigningAuthenticationType = authType; }

    constexpr void setValidityLeeway(std::chrono::seconds leeway) { mValidityLeeway = leeway; }

    constexpr void setValidityTime(std::chrono::system_clock::time_point validityTime) { mValidityTime = validityTime; }

    constexpr void setSigningTime(std::chrono::system_clock::time_point signingTime) { mSigningTime = signingTime; }

    Result<> initMojangPublicKeyBlocking(std::size_t timeoutSeconds);

    std::future<Result<>> initMojangPublicKeyAsync(std::size_t timeoutSeconds);

    Result<> initForSign(AuthenticationType authType);
};

} // namespace sculk::protocol::inline abi_v975