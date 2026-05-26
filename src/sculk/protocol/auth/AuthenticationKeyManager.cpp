// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/auth/AuthenticationKeyManager.hpp"
#include "ssl/ES384.hpp"
#include "ssl/RS256.hpp"
#include <httplib.h>
#include <random>
#include <sculk/reflection/jsonc/reflection.hpp>

namespace sculk::protocol::inline abi_v975 {

constexpr std::string_view MOJANG_PUBLIC_KEY_PEM =
    "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAECRXueJeTDqNRRgJi/vlRufByu/2G0i2Ebt6YMar5QX/R0DIIyrJMcUpruK4QveTfJSTp3Shlq4Gk34cD/"
    "4GUWwkv0DVuzeuB+tXija7HBxii03NHDbPAD0AKnLr2wdAp";

std::chrono::system_clock::time_point AuthenticationKeyManager::getValidityTime() const {
    return mValidityTime.value_or(std::chrono::system_clock::now());
}

const std::vector<std::string>& AuthenticationKeyManager::getLegacyCertificateChainPublicKeyPems() const {
    return mLegacyCertificateChainPublicKeyPems;
}

std::optional<std::string_view>
AuthenticationKeyManager::getLoginTokenPublicKeyPemByKeyId(const std::string& keyId) const {
    auto it = mLoginTokenPublicKeysPemByKeyId.find(keyId);
    if (it != mLoginTokenPublicKeysPemByKeyId.end()) {
        return it->second;
    }
    return {};
}

std::string_view AuthenticationKeyManager::getLoginTokenExpectedIssuer() const { return mLoginTokenExpectedIssuer; }

std::string_view AuthenticationKeyManager::getLoginTokenExpectedPlayFabTitle() const {
    return mLoginTokenExpectedPlayFabTitle;
}

Result<AuthenticationKeyManager::KeyPair> AuthenticationKeyManager::generateRandomES384KeyPair() const {
    std::string privateKeyPem{};
    std::string publicKeyPem{};
    if (!es384::generateES384KeyPair(publicKeyPem, privateKeyPem)) {
        return error_utils::makeError("Failed to generate ES384 key pair");
    }
    return KeyPair{std::move(publicKeyPem), std::move(privateKeyPem)};
}

Result<AuthenticationKeyManager::KeyPair> AuthenticationKeyManager::generateRandomRS256KeyPair() const {
    std::string privateKeyPem{};
    std::string publicKeyPem{};
    if (!rs256::generateRS256KeyPair(publicKeyPem, privateKeyPem)) {
        return error_utils::makeError("Failed to generate RS256 key pair");
    }
    return KeyPair{std::move(publicKeyPem), std::move(privateKeyPem)};
}

std::chrono::system_clock::time_point AuthenticationKeyManager::getSigningTime() const {
    return mSigningTime.value_or(std::chrono::system_clock::now());
}

bool AuthenticationKeyManager::legacyCertificateChainSigningInitialized(AuthenticationType authType) const {
    if (authType == AuthenticationType::Full) {
        return mLegacyCertificateClientKeyPair.has_value() && mLegacyCertificateMojangKeyPair.has_value()
            && mLegacyCertificateLoginKeyPair.has_value();
    } else if (authType == AuthenticationType::SelfSigned) {
        return mLegacyCertificateLoginKeyPair.has_value();
    }
    return false;
}

void AuthenticationKeyManager::setLegacyCertificateChainClientKeyPair(
    std::string_view publicKeyPem,
    std::string_view privateKeyPem
) {
    mLegacyCertificateClientKeyPair = KeyPair{std::string(publicKeyPem), std::string(privateKeyPem)};
}

void AuthenticationKeyManager::setLegacyCertificateChainMojangKeyPair(
    std::string_view publicKeyPem,
    std::string_view privateKeyPem
) {
    mLegacyCertificateMojangKeyPair = KeyPair{std::string(publicKeyPem), std::string(privateKeyPem)};
}

void AuthenticationKeyManager::setLegacyCertificateChainLoginKeyPair(
    std::string_view publicKeyPem,
    std::string_view privateKeyPem
) {
    mLegacyCertificateLoginKeyPair = KeyPair{std::string(publicKeyPem), std::string(privateKeyPem)};
}

Result<AuthenticationKeyManager::KeyPair> AuthenticationKeyManager::getLegacyCertificateChainClientKeyPair() const {
    if (mLegacyCertificateClientKeyPair) {
        return *mLegacyCertificateClientKeyPair;
    }
    return error_utils::makeError("Client key pair not set");
}

Result<AuthenticationKeyManager::KeyPair> AuthenticationKeyManager::getLegacyCertificateChainMojangKeyPair() const {
    if (mLegacyCertificateMojangKeyPair) {
        return *mLegacyCertificateMojangKeyPair;
    }
    return error_utils::makeError("Mojang key pair not set");
}

Result<AuthenticationKeyManager::KeyPair> AuthenticationKeyManager::getLegacyCertificateChainLoginKeyPair() const {
    if (mLegacyCertificateLoginKeyPair) {
        return *mLegacyCertificateLoginKeyPair;
    }
    return error_utils::makeError("Login key pair not set");
}

bool AuthenticationKeyManager::loginTokenSigningInitialized(AuthenticationType authType) const {
    if (authType == AuthenticationType::Full) {
        return mLoginTokenKeyPairsAndKeyId.has_value();
    } else if (authType == AuthenticationType::SelfSigned) {
        return mSelfSignedLoginTokenKeyPair.has_value();
    }
    return false;
}

std::string AuthenticationKeyManager::generateRandomKeyId() const {
    // Example Key ID: 6CD621632CEE45A16374B30445B1FB9B77EC7BF7
    static constexpr char HEX_TABLE[] = "0123456789ABCDEF";

    thread_local std::mt19937_64       generator(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 255);

    std::string keyId{};
    keyId.reserve(40);

    do {
        keyId.clear();
        for (int i = 0; i < 20; ++i) {
            const int value = dist(generator);
            keyId.push_back(HEX_TABLE[(value >> 4) & 0x0F]);
            keyId.push_back(HEX_TABLE[value & 0x0F]);
        }
    } while (mLoginTokenPublicKeysPemByKeyId.find(keyId) != mLoginTokenPublicKeysPemByKeyId.end());

    return keyId;
}

Result<> AuthenticationKeyManager::generateAndSetLoginTokenKeyPairFull() {
    auto keyPair = generateRandomRS256KeyPair();
    if (!keyPair) {
        return error_utils::makeError("Failed to generate login token key pair");
    }
    mLoginTokenKeyPairsAndKeyId = std::make_pair(generateRandomKeyId(), *keyPair);
    mSigningAuthenticationType  = AuthenticationType::Full;
    return {};
}

Result<> AuthenticationKeyManager::generateAndSetLoginTokenKeyPairSelfSigned() {
    auto keyPair = generateRandomES384KeyPair();
    if (!keyPair) {
        return error_utils::makeError("Failed to generate login token key pair");
    }
    mSelfSignedLoginTokenKeyPair = *keyPair;
    mSigningAuthenticationType   = AuthenticationType::SelfSigned;
    return {};
}

void AuthenticationKeyManager::setLoginTokenKeyPairFull(
    const std::string& keyId,
    std::string_view   publicKeyPem,
    std::string_view   privateKeyPem
) {
    mLoginTokenKeyPairsAndKeyId = std::make_pair(keyId, KeyPair{std::string(publicKeyPem), std::string(privateKeyPem)});
}

void AuthenticationKeyManager::setLoginTokenKeyPairSelfSigned(
    std::string_view publicKeyPem,
    std::string_view privateKeyPem
) {
    mSelfSignedLoginTokenKeyPair = KeyPair{std::string(publicKeyPem), std::string(privateKeyPem)};
}

Result<AuthenticationKeyManager::KeyPair>
AuthenticationKeyManager::getFullLoginTokenKeyPairAndKeyId(std::string& outKeyId) const {
    if (mLoginTokenKeyPairsAndKeyId) {
        outKeyId = mLoginTokenKeyPairsAndKeyId->first;
        return mLoginTokenKeyPairsAndKeyId->second;
    }
    return error_utils::makeError("Login token key pair not set");
}

Result<AuthenticationKeyManager::KeyPair> AuthenticationKeyManager::getSelfSignedLoginTokenKeyPair() const {
    if (mSelfSignedLoginTokenKeyPair) {
        return *mSelfSignedLoginTokenKeyPair;
    }
    return error_utils::makeError("Login token key pair not set");
}

void AuthenticationKeyManager::addLoginTokenPublicKeyPemByKeyId(
    const std::string& keyId,
    const std::string& publicKeyPem
) {
    mLoginTokenPublicKeysPemByKeyId[keyId] = publicKeyPem;
}

void AuthenticationKeyManager::addLegacyCertificateChainPublicKeyPem(std::string_view publicKeyPem) {
    mLegacyCertificateChainPublicKeyPems.emplace_back(publicKeyPem);
}

Result<AuthenticationKeyManager::KeyPair> AuthenticationKeyManager::getClientPropertiesKeyPair() const {
    if (mSigningAuthenticationType == AuthenticationType::Full) {
        return getLegacyCertificateChainClientKeyPair();
    } else if (mSigningAuthenticationType == AuthenticationType::SelfSigned) {
        return getLegacyCertificateChainLoginKeyPair();
    }
    return error_utils::makeError("Unsupported authentication type for getting client properties key pair");
}

Result<> AuthenticationKeyManager::generateAndSetLegacyFullCertificateChainKeyPairs() {
    auto clientResult = generateRandomES384KeyPair();
    if (!clientResult) {
        return error_utils::makeError("Failed to generate ES384 key pair");
    }
    mLegacyCertificateClientKeyPair = std::move(*clientResult);

    auto mojangResult = generateRandomES384KeyPair();
    if (!mojangResult) {
        return error_utils::makeError("Failed to generate ES384 key pair");
    }
    mLegacyCertificateMojangKeyPair = std::move(*mojangResult);

    auto loginResult = generateRandomES384KeyPair();
    if (!loginResult) {
        return error_utils::makeError("Failed to generate ES384 key pair");
    }
    mLegacyCertificateLoginKeyPair = std::move(*loginResult);

    mSigningAuthenticationType = AuthenticationType::Full;
    return {};
}

Result<> AuthenticationKeyManager::generateAndSetLegacySelfSignedCertificateChainKeyPairs() {
    auto loginResult = generateRandomES384KeyPair();
    if (!loginResult) {
        return error_utils::makeError("Failed to generate ES384 key pair");
    }
    mLegacyCertificateLoginKeyPair = std::move(*loginResult);

    mSigningAuthenticationType = AuthenticationType::SelfSigned;
    return {};
}

struct MojangServiceFetchResult {
    struct {
        struct {
            struct {
                struct {
                    std::string serviceUri{};
                    std::string issuer{};
                    std::string playfabTitleId{};
                    std::string eduPlayFabTitleId{};
                } prod;
            } auth;
        } serviceEnvironments;
    } result;
};

struct MojangConfigFetchResult {
    std::string issuer{};
};

struct MojangPublicKeyFetchResult {
    struct KeyInfo {
        std::string kty{};
        std::string use{};
        std::string kid{};
        std::string x5t{};
        std::string n{};
        std::string e{};
    };
    std::vector<KeyInfo> keys{};
};

Result<> AuthenticationKeyManager::initMojangPublicKeyBlocking(std::size_t timeoutSeconds) {
    mVerifyAuthenticationType = AuthenticationType::Full;
    mValidityLeeway           = std::chrono::seconds(60);
    mLegacyCertificateChainPublicKeyPems.emplace_back(MOJANG_PUBLIC_KEY_PEM);

    // https://client.discovery.minecraft-services.net/api/v1.0/discovery/MinecraftPE/builds/1.0.0.0
    httplib::Client serviceClient("https://client.discovery.minecraft-services.net");
    serviceClient.set_connection_timeout(timeoutSeconds);
    serviceClient.set_read_timeout(timeoutSeconds);
    serviceClient.set_write_timeout(timeoutSeconds);
    httplib::Result serviceRes = serviceClient.Get("/api/v1.0/discovery/MinecraftPE/builds/1.0.0.0");
    if (!serviceRes || serviceRes->status != 200) {
        return error_utils::makeError("Failed to fetch Mojang service from Internet");
    }
    auto serviceJson = jsonc::json::parse(serviceRes->body);
    if (!serviceJson) {
        return error_utils::makeError("Failed to parse Mojang service response JSON");
    }
    MojangServiceFetchResult fetchResult{};
    if (auto status = reflection::jsonc::deserialize(fetchResult, *serviceJson); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(
            std::format("Failed to deserialize Mojang service response JSON: {}", status.error())
        );
#else
        return error_utils::makeError("Failed to deserialize Mojang service response JSON");
#endif
    }
    mLoginTokenExpectedPlayFabTitle = std::move(fetchResult.result.serviceEnvironments.auth.prod.playfabTitleId);

    httplib::Client keyClient(fetchResult.result.serviceEnvironments.auth.prod.serviceUri);
    keyClient.set_connection_timeout(timeoutSeconds);
    keyClient.set_read_timeout(timeoutSeconds);
    keyClient.set_write_timeout(timeoutSeconds);
    // {auth service base URL}/.well-known/keys
    httplib::Result keyRes = keyClient.Get("/.well-known/keys");
    if (!keyRes || keyRes->status != 200) {
        return error_utils::makeError("Failed to fetch Mojang public key from Internet");
    }
    auto keyJson = jsonc::json::parse(keyRes->body);
    if (!keyJson) {
        return error_utils::makeError("Failed to parse Mojang public key response JSON");
    }
    MojangPublicKeyFetchResult keyFetchResult{};
    if (auto status = reflection::jsonc::deserialize(keyFetchResult, *keyJson); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(
            std::format("Failed to deserialize Mojang public key response JSON: {}", status.error())
        );
#else
        return error_utils::makeError("Failed to deserialize Mojang public key response JSON");
#endif
    }
    if (keyFetchResult.keys.empty()) {
        return error_utils::makeError("Mojang public key response JSON does not contain any keys");
    }
    for (const auto& keyInfo : keyFetchResult.keys) {
        if (keyInfo.kty == "RSA" && keyInfo.use == "sig") {
            std::string pem{};
            if (!rs256::jwkRsaPublicKeyToPem(keyInfo.n, keyInfo.e, pem)) {
                return error_utils::makeError("Failed to convert Mojang public key from JWK to PEM format");
            }
            mLoginTokenPublicKeysPemByKeyId.emplace(keyInfo.kid, std::move(pem));
        }
    }

    // {auth service base URL}/.well-known/openid-configuration
    httplib::Result issuerRes = keyClient.Get("/.well-known/openid-configuration");
    if (!issuerRes || issuerRes->status != 200) {
        return error_utils::makeError("Failed to fetch Mojang configuration from Internet");
    }
    auto issuerJson = jsonc::json::parse(issuerRes->body);
    if (!issuerJson) {
        return error_utils::makeError("Failed to parse Mojang configuration response JSON");
    }
    MojangConfigFetchResult configFetchResult{};
    if (auto status = reflection::jsonc::deserialize(configFetchResult, *issuerJson); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(
            std::format("Failed to deserialize Mojang configuration response JSON: {}", status.error())
        );
#else
        return error_utils::makeError("Failed to deserialize Mojang configuration response JSON");
#endif
    }
    mLoginTokenExpectedIssuer = std::move(configFetchResult.issuer);

    return {};
}

std::future<Result<>> AuthenticationKeyManager::initMojangPublicKeyAsync(std::size_t timeoutSeconds) {
    return std::async(std::launch::async, [this, timeoutSeconds]() {
        return initMojangPublicKeyBlocking(timeoutSeconds);
    });
}

Result<> AuthenticationKeyManager::initForSign(AuthenticationType authType) {
    if (authType == AuthenticationType::Full) {
        if (auto result = generateAndSetLoginTokenKeyPairFull(); !result) {
            return result;
        }
        return generateAndSetLegacyFullCertificateChainKeyPairs();
    } else if (authType == AuthenticationType::SelfSigned) {
        if (auto result = generateAndSetLoginTokenKeyPairSelfSigned(); !result) {
            return result;
        }
        return generateAndSetLegacySelfSignedCertificateChainKeyPairs();
    }
    return error_utils::makeError("Unsupported authentication type for initialization for signing");
}

} // namespace sculk::protocol::inline abi_v975