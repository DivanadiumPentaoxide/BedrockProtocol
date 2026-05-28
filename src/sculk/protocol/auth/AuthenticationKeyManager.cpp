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
    mExpectedPlayFabTitle = std::move(fetchResult.result.serviceEnvironments.auth.prod.playfabTitleId);

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
            mPublicKeysPemByKeyId.emplace(keyInfo.kid, std::move(pem));
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
    mExpectedIssuer = std::move(configFetchResult.issuer);

    return {};
}

std::future<Result<>> AuthenticationKeyManager::initMojangPublicKeyAsync(std::size_t timeoutSeconds) {
    return std::async(std::launch::async, [this, timeoutSeconds]() {
        return initMojangPublicKeyBlocking(timeoutSeconds);
    });
}

} // namespace sculk::protocol::inline abi_v975