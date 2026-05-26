// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/auth/ConnectionRequest.hpp"
#include "sculk/protocol/utility/Base64Url.hpp"
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include "ssl/PemHelper.hpp"
#include <limits>
#include <random>
#include <sculk/reflection/jsonc/reflection.hpp>

namespace sculk::reflection::jsonc {
template <>
struct serializer<sculk::protocol::AuthenticationType> {
    static int to_signed(const sculk::protocol::AuthenticationType& t) { return static_cast<int>(t); }
    static std::optional<sculk::protocol::AuthenticationType> from_signed(int v) {
        if (v < static_cast<int>(sculk::protocol::AuthenticationType::Full)
            || v > static_cast<int>(sculk::protocol::AuthenticationType::SelfSigned)) {
            return std::nullopt;
        }
        return static_cast<sculk::protocol::AuthenticationType>(v);
    }
};
} // namespace sculk::reflection::jsonc

namespace sculk::protocol::inline abi_v975 {

std::optional<std::string> ConnectionRequest::getXboxLiveID() const {
    std::string xuid{};
    if (mLoginToken && mLoginToken->mHeader.alg == "RS256") {
        xuid = mLoginToken->mPayload.xid;
    }
    if (mLegacyCertificateChain && mLegacyCertificateChain->mClientCertificate.has_value()
        && mLegacyCertificateChain->mMojangCertificate.has_value()) {
        xuid = mLegacyCertificateChain->mLoginCertificate.mPayload.extraData->XUID;
    }
    if (!xuid.empty() && Identity::validateXuid(xuid)) {
        return xuid;
    }
    return std::nullopt;
}

std::string ConnectionRequest::getXboxLiveName() const {
    if (mLoginToken) {
        return mLoginToken->mPayload.xname;
    }
    if (mLegacyCertificateChain) {
        return mLegacyCertificateChain->mLoginCertificate.mPayload.extraData->displayName;
    }
    return {};
}

Identity ConnectionRequest::getIdentity() const {
    auto xuid = getXboxLiveID();
    if (xuid && Identity::validateXuid(*xuid)) {
        return Identity::fromXuid(*xuid);
    }
    auto selfSignedId = mClientProperties.mPayload.mSelfSignedId;
    if (Identity::validateString(selfSignedId)) {
        return Identity::fromString(selfSignedId);
    }
    return {};
}

std::string ConnectionRequest::getPlayFabID() const {
    if (mLoginToken) {
        return mLoginToken->mPayload.mid;
    }
    return mClientProperties.mPayload.mPlayFabId;
}

Result<AuthenticationType> ConnectionRequest::verify(const AuthenticationKeyManager& authenticationKeyManager) const {
    if (mLoginToken) {
        if (auto status = mClientProperties.verify(mLoginToken->getClientPublicKey()); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
            return error_utils::makeError(
                std::format("Client properties verification failed: {}", status.error().mMessage)
            );
#else
            return error_utils::makeError("Client properties verification failed");
#endif
        }
        return mLoginToken->verify(authenticationKeyManager);
    }

    if (mLegacyCertificateChain) {
        if (auto status = mClientProperties.verify(mLegacyCertificateChain->getClientPublicKey()); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
            return error_utils::makeError(
                std::format("Client properties verification failed: {}", status.error().mMessage)
            );
#else
            return error_utils::makeError("Client properties verification failed");
#endif
        }
        return mLegacyCertificateChain->verify(authenticationKeyManager);
    }

    return error_utils::makeError("ConnectionRequest must have either a login token or a legacy certificate chain");
}

namespace {

template <typename T = std::int64_t>
inline T randomInt() {
    thread_local std::mt19937_64     generator(std::random_device{}());
    std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    return dist(generator);
}

inline Identity randomIdentity() {
    return Identity{.mHighBits = randomInt<std::uint64_t>(), .mLowBits = randomInt<std::uint64_t>()};
}

inline Result<>
ensureAndFillAllFieldsFull(ConnectionRequest& request, const AuthenticationKeyManager& publicKeyManager) {
    if (!request.mLoginToken) {
        request.mLoginToken.emplace();
    } else if (!request.mLegacyCertificateChain) {
        request.mLegacyCertificateChain = LegacyCertificateChain{
            .mLoginCertificate = Certificate{
                .mPayload = {
                    .randomNonce = randomInt(),
                    .iss         = "Mojang",
                    .iat         = 0,
                    .extraData   = Certificate::ExtraData{
                        .identity    = request.getIdentity().toString(),
                        .displayName = request.getXboxLiveName(),
                        .XUID        = request.getXboxLiveID().value_or(""),
                        .sandBoxId   = "RETAIL"
                    }
                }
            }
        };
        request.mClientProperties.mPayload.mPlayFabId = request.mLoginToken->mPayload.mid;
    }

    auto& loginTokenPayload = request.mLoginToken->mPayload;
    loginTokenPayload.sub   = ""; // Unknown
    loginTokenPayload.ipt   = ""; // Unknown
    loginTokenPayload.mid   = request.getPlayFabID();
    loginTokenPayload.tid   = std::string(publicKeyManager.getLoginTokenExpectedPlayFabTitle());
    loginTokenPayload.pfcd  = 0; // Unknown
    loginTokenPayload.cpk =
        pem_helper::stripPemMarkersAndCompact(publicKeyManager.getClientPropertiesKeyPair()->mPublicKeyPem);
    loginTokenPayload.ap    = 15; // Unknown
    loginTokenPayload.xid   = request.getXboxLiveID().value_or("");
    loginTokenPayload.xname = request.getXboxLiveName();
    loginTokenPayload.iss   = std::string(publicKeyManager.getLoginTokenExpectedIssuer());
    loginTokenPayload.aud   = "api://auth-minecraft-services/multiplayer";

    if (!request.mLegacyCertificateChain->mClientCertificate) {
        request.mLegacyCertificateChain->mClientCertificate = Certificate{.mPayload = {.certificateAuthority = true}};
    }

    if (!request.mLegacyCertificateChain->mMojangCertificate) {
        request.mLegacyCertificateChain->mMojangCertificate = Certificate{
            .mPayload = {.certificateAuthority = true, .randomNonce = randomInt(), .iss = "Mojang", .iat = 0}
        };
    }

    return {};
}

inline Result<>
ensureAndFillAllFieldsSelfSigned(ConnectionRequest& request, const AuthenticationKeyManager& publicKeyManager) {
    if (!request.mLoginToken) {
        request.mLoginToken.emplace();
    } else if (!request.mLegacyCertificateChain) {
        request.mLegacyCertificateChain = LegacyCertificateChain{
            .mLoginCertificate = Certificate{
                .mPayload = {
                    .randomNonce = randomInt(),
                    .iss         = "Mojang",
                    .iat         = 0,
                    .extraData   = Certificate::ExtraData{
                        .identity    = request.getIdentity().toString(),
                        .displayName = request.getXboxLiveName(),
                        .XUID        = "",      // Empty XUID
                        .sandBoxId   = "RETAIL" // Always "RETAIL"
                    }
                }
            }
        };
        request.mClientProperties.mPayload.mPlayFabId = request.mLoginToken->mPayload.mid;
    }

    auto& loginTokenPayload = request.mLoginToken->mPayload;

    loginTokenPayload.mid = request.getPlayFabID();
    loginTokenPayload.cpk =
        pem_helper::stripPemMarkersAndCompact(publicKeyManager.getClientPropertiesKeyPair()->mPublicKeyPem);
    loginTokenPayload.ap      = 0; // Unknown
    loginTokenPayload.xid     = request.getXboxLiveID().value_or("");
    loginTokenPayload.xname   = request.getXboxLiveName();
    loginTokenPayload.aud     = "api://auth-minecraft-services/multiplayer";
    loginTokenPayload.leguuid = randomIdentity().toString(); // Unknown
    loginTokenPayload.nid     = "";                          // Unknown
    loginTokenPayload.nname   = "";                          // Unknown
    loginTokenPayload.pid     = "";                          // Unknown
    loginTokenPayload.pname   = "";                          // Unknown

    return {};
}

} // namespace

Result<> ConnectionRequest::sign(const AuthenticationKeyManager& authenticationKeyManager) {
    if (!mLoginToken && !mLegacyCertificateChain) {
        return error_utils::makeError(
            "ConnectionRequest must have either a login token or a legacy certificate chain to sign"
        );
    }

    mAuthenticationType = authenticationKeyManager.getSigningAuthenticationType();
    if (mAuthenticationType == AuthenticationType::Full) {
        if (auto status = ensureAndFillAllFieldsFull(*this, authenticationKeyManager); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
            return error_utils::makeError(
                std::format("Failed to ensure and fill all fields for full authentication: {}", status.error().mMessage)
            );
#else
            return error_utils::makeError("Failed to ensure and fill all fields for full authentication");
#endif
        }
    } else if (mAuthenticationType == AuthenticationType::SelfSigned) {
        if (auto status = ensureAndFillAllFieldsSelfSigned(*this, authenticationKeyManager); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
            return error_utils::makeError(
                std::format(
                    "Failed to ensure and fill all fields for self-signed authentication: {}",
                    status.error().mMessage
                )
            );
#else
            return error_utils::makeError("Failed to ensure and fill all fields for self-signed authentication");
#endif
        }
    }

    if (mLoginToken && authenticationKeyManager._loginTokenSigningInitialized(mAuthenticationType)) {
        if (auto status = mLoginToken->sign(authenticationKeyManager); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
            return error_utils::makeError(std::format("Login token signing failed: {}", status.error().mMessage));
#else
            return error_utils::makeError("Login token signing failed");
#endif
        }
    }

    if (mLegacyCertificateChain
        && authenticationKeyManager._legacyCertificateChainSigningInitialized(mAuthenticationType)) {
        if (auto status = mLegacyCertificateChain->sign(authenticationKeyManager); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
            return error_utils::makeError(
                std::format("Legacy certificate chain signing failed: {}", status.error().mMessage)
            );
#else
            return error_utils::makeError("Legacy certificate chain signing failed");
#endif
        }
    }

    if (auto status = mClientProperties.sign(authenticationKeyManager); !status) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(std::format("Client properties signing failed: {}", status.error().mMessage));
#else
        return error_utils::makeError("Client properties signing failed");
#endif
    }

    return {};
}

std::string ConnectionRequest::toString() const {
    std::vector<std::byte> buffer{};
    BinaryStream           stream(buffer);

    jsonc::json authJson = {
        {"AuthenticationType", static_cast<int>(mAuthenticationType)     },
        {"Token",              mLoginToken ? mLoginToken->toString() : ""}
    };
    if (mLegacyCertificateChain) {
        authJson["Certificate"] = mLegacyCertificateChain->toString();
    }

    stream.writeLongString(authJson.dump(-1));
    stream.writeLongString(mClientProperties.toString());

    std::string result{};
    result.resize_and_overwrite(buffer.size(), [&buffer](char* data, std::size_t size) {
        std::memcpy(data, buffer.data(), size);
        return size;
    });

    return result;
}

#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
#define SCULK_CONNECTION_REQUEST_DESERIALIZE_REQUIRED(FIELD_NAME, VALUE)                                               \
    if (!authJson.contains(FIELD_NAME)) {                                                                              \
        return error_utils::makeError("Authentication JSON does not contain a valid '" FIELD_NAME "' field");          \
    }                                                                                                                  \
    if (auto status = reflection::jsonc::deserialize<false, false>(VALUE, authJson[FIELD_NAME], options); !status) {   \
        return error_utils::makeError(                                                                                 \
            std::format("Failed to deserialize '{}' field in authentication JSON: {}", FIELD_NAME, status.error())     \
        );                                                                                                             \
    }

#define SCULK_CONNECTION_REQUEST_DESERIALIZE_OPTIONAL(FIELD_NAME, VALUE)                                               \
    if (authJson.contains(FIELD_NAME)) {                                                                               \
        if (auto status = reflection::jsonc::deserialize<false, false>(VALUE, authJson[FIELD_NAME], options);          \
            !status) {                                                                                                 \
            return error_utils::makeError(                                                                             \
                std::format("Failed to deserialize '{}' field in authentication JSON: {}", FIELD_NAME, status.error()) \
            );                                                                                                         \
        }                                                                                                              \
    }
#else
#define SCULK_CONNECTION_REQUEST_DESERIALIZE_REQUIRED(FIELD_NAME, VALUE)                                               \
    if (!authJson.contains(FIELD_NAME)) {                                                                              \
        return error_utils::makeError("Authentication JSON does not contain a valid '" FIELD_NAME "' field");          \
    }                                                                                                                  \
    if (!reflection::jsonc::deserialize<false, false>(VALUE, authJson[FIELD_NAME], options)) {                         \
        return error_utils::makeError("Failed to deserialize '" FIELD_NAME "' field in authentication JSON");          \
    }

#define SCULK_CONNECTION_REQUEST_DESERIALIZE_OPTIONAL(FIELD_NAME, VALUE)                                               \
    if (authJson.contains(FIELD_NAME)) {                                                                               \
        if (!reflection::jsonc::deserialize<false, false>(VALUE, authJson[FIELD_NAME], options)) {                     \
            return error_utils::makeError("Failed to deserialize '" FIELD_NAME "' field in authentication JSON");      \
        }                                                                                                              \
    }
#endif

Result<ConnectionRequest> ConnectionRequest::fromString(std::string_view rawRequest) {
    ReadOnlyBinaryStream stream(rawRequest);

    std::string authJsonStr{};
    if (!stream.readLongString(authJsonStr)) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(
            std::format(
                "Read ConnectionRequest authentication JSON overflowed: mReadPointer={}, size={}",
                stream.getPosition(),
                stream.size()
            )
        );
#else
        return error_utils::makeError("Read ConnectionRequest authentication JSON overflowed");
#endif
    }

    auto authJsonOpt = jsonc::json::parse(authJsonStr);
    if (!authJsonOpt) {
        return error_utils::makeError("Failed to parse authentication JSON from ConnectionRequest");
    }

    const auto&                       authJson = *authJsonOpt;
    static reflection::jsonc::options options{.indent = -1, .allow_trailing_comma = false};

    AuthenticationType authType{};
    SCULK_CONNECTION_REQUEST_DESERIALIZE_REQUIRED("AuthenticationType", authType);
    std::optional<std::string> legacyCertificate{};
    SCULK_CONNECTION_REQUEST_DESERIALIZE_OPTIONAL("Certificate", legacyCertificate);
    std::string loginToken{};
    SCULK_CONNECTION_REQUEST_DESERIALIZE_REQUIRED("Token", loginToken);

    std::string clientProperties{};
    if (!stream.readLongString(clientProperties)) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(
            std::format(
                "Read ConnectionRequest client properties overflowed: mReadPointer={}, size={}",
                stream.getPosition(),
                stream.size()
            )
        );
#else
        return error_utils::makeError("Read ConnectionRequest client properties overflowed");
#endif
    }

    return ConnectionRequest::create(
        authType,
        std::move(legacyCertificate),
        loginToken.empty() ? std::nullopt : std::make_optional(std::move(loginToken)),
        std::move(clientProperties)
    );
}

Result<ConnectionRequest> ConnectionRequest::create(
    AuthenticationType           authenticationType,
    std::optional<std::string>&& legacyCertificateChainString,
    std::optional<std::string>&& loginTokenString,
    std::string&&                clientPropertiesString
) {
    std::optional<LegacyCertificateChain> legacyCertificateChain{};
    if (legacyCertificateChainString && *legacyCertificateChainString != "{\"chain\":[\"..\"]}\n") {
        auto certChain = LegacyCertificateChain::fromString(*legacyCertificateChainString);
        if (!certChain) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
            return error_utils::makeError(
                std::format("Failed to parse legacy certificate chain: {}", certChain.error().mMessage)
            );
#else
            return error_utils::makeError("Failed to parse legacy certificate chain");
#endif
        }
        legacyCertificateChain = std::move(*certChain);
    }

    std::optional<LoginToken> loginToken{};
    if (loginTokenString) {
        auto token = LoginToken::fromString(*loginTokenString);
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        if (!token) {
            return error_utils::makeError(std::format("Failed to parse login token: {}", token.error().mMessage));
        }
#else
        if (!token) {
            return error_utils::makeError("Failed to parse login token");
        }
#endif
        loginToken = std::move(*token);
    }

    if (!legacyCertificateChain && !loginToken) {
        return error_utils::makeError("ConnectionRequest must have either a legacy certificate chain or a login token");
    }

    auto clientProperties = ClientProperties::fromString(std::move(clientPropertiesString));
    if (!clientProperties) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(
            std::format("Failed to parse client properties: {}", clientProperties.error().mMessage)
        );
#else
        return error_utils::makeError("Failed to parse client properties");
#endif
    }

    return ConnectionRequest{
        .mAuthenticationType     = authenticationType,
        .mLegacyCertificateChain = std::move(legacyCertificateChain),
        .mLoginToken             = std::move(loginToken),
        .mClientProperties       = std::move(*clientProperties)
    };
}

} // namespace sculk::protocol::inline abi_v975
