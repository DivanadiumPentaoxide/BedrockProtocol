// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/HandShakeToken.hpp"
#include "../ssl/ES384.hpp"
#include "../ssl/RS256.hpp"
#include <sculk/reflection/jsonc/reflection.hpp>

namespace sculk::protocol::inline abi_v975 {

#define SCULK_HANDSHAKE_TOKEN_SERIALIZE_OPTION_INIT() static reflection::jsonc::options options{.indent = -1}

#define SCULK_HANDSHAKE_TOKEN_CREATE_JSON(PART, DATA)                                                                  \
    jsonc::json PART##Json = jsonc::json::object();                                                                    \
    const auto& PART       = DATA;

#define SCULK_HANDSHAKE_TOKEN_SERIALIZE(PART, FIELD)                                                                   \
    auto FIELD         = reflection::jsonc::serialize<false, false>(PART.FIELD, options);                              \
    PART##Json[#FIELD] = FIELD;


#define SCULK_HANDSHAKE_TOKEN_PARSE_JSON(PART, RAW)                                                                    \
    auto PART##JsonStr = base64url::decodeChecked(RAW);                                                                \
    if (!PART##JsonStr) {                                                                                              \
        return error_utils::makeError("Failed to decode login token " #PART);                                          \
    }                                                                                                                  \
    auto PART##JsonOpt = jsonc::json::parse(*PART##JsonStr);                                                           \
    if (!PART##JsonOpt) {                                                                                              \
        return error_utils::makeError("Failed to parse login token " #PART " JSON");                                   \
    }                                                                                                                  \
    const auto& PART##Json = *PART##JsonOpt;

#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
#define SCULK_HANDSHAKE_TOKEN_DESERIALIZE(PART, FIELD)                                                                 \
    if (!PART##Json.contains(#FIELD)) {                                                                                \
        return error_utils::makeError("Login token " #PART " JSON does not contain a valid field '" #FIELD "'");       \
    }                                                                                                                  \
    if (auto status = reflection::jsonc::deserialize<false, false>(PART.FIELD, PART##Json[#FIELD], options);           \
        !status) {                                                                                                     \
        return error_utils::makeError(                                                                                 \
            std::format("Failed to deserialize login token {} field '{}': {}", #PART, #FIELD, status.error())          \
        );                                                                                                             \
    }
#else
#define SCULK_HANDSHAKE_TOKEN_DESERIALIZE(PART, FIELD)                                                                 \
    if (!PART##Json.contains(#FIELD)) {                                                                                \
        return error_utils::makeError("Login token " #PART " JSON does not contain a valid field '" #FIELD "'");       \
    }                                                                                                                  \
    if (!reflection::jsonc::deserialize<false, false>(PART.FIELD, PART##Json[#FIELD], options)) {                      \
        return error_utils::makeError("Failed to deserialize login token " #PART " field '" #FIELD "'");               \
    }
#endif

Result<> HandShakeToken::verify() const {
    std::string signingInput = std::format("{}.{}", mRawHeader, mRawPayload);

    if (mHeader.alg != "ES384") {
        return error_utils::makeError("Unsupported algorithm in handshake token header");
    }

    if (!es384::verifyES384Signature(signingInput, mSignature, mHeader.x5u)) {
        return error_utils::makeError("Failed to verify login token signature");
    }
    return {};
}

Result<> HandShakeToken::sign(const PemKeyPair& localKeyPair) {
    SCULK_HANDSHAKE_TOKEN_SERIALIZE_OPTION_INIT();

    mHeader.alg = "ES384";
    mHeader.x5u = pem_helper::stripPemMarkersAndCompact(localKeyPair.mPublicKeyPem);

    SCULK_HANDSHAKE_TOKEN_CREATE_JSON(header, mHeader);
    SCULK_HANDSHAKE_TOKEN_SERIALIZE(header, alg);
    SCULK_HANDSHAKE_TOKEN_SERIALIZE(header, x5u);
    mRawHeader = base64url::encode(headerJson.dump(-1));

    SCULK_HANDSHAKE_TOKEN_CREATE_JSON(payload, mPayload);
    SCULK_HANDSHAKE_TOKEN_SERIALIZE(payload, salt);
    mRawPayload = base64url::encode(payloadJson.dump(-1));

    auto signingInput = std::format("{}.{}", mRawHeader, mRawPayload);
    if (!es384::signES384Signature(signingInput, localKeyPair.mPrivateKeyPem, mSignature)) {
        return error_utils::makeError("Failed to sign login token with ES384");
    }
    return {};
}

Result<HandShakeToken> HandShakeToken::fromString(std::string_view rawLoginToken) {
    SCULK_HANDSHAKE_TOKEN_SERIALIZE_OPTION_INIT();

    const auto first = rawLoginToken.find('.');
    const auto last  = rawLoginToken.rfind('.');

    if (first == std::string::npos || last == std::string::npos || first == last) {
        return error_utils::makeError("Invalid login token format: expected three parts separated by dots");
    }

    auto   rawHeader = rawLoginToken.substr(0, first);
    Header header{};
    SCULK_HANDSHAKE_TOKEN_PARSE_JSON(header, rawHeader);
    SCULK_HANDSHAKE_TOKEN_DESERIALIZE(header, alg);
    SCULK_HANDSHAKE_TOKEN_DESERIALIZE(header, x5u);

    auto    rawPayload = rawLoginToken.substr(first + 1, last - first - 1);
    Payload payload{};
    SCULK_HANDSHAKE_TOKEN_PARSE_JSON(payload, rawPayload);
    SCULK_HANDSHAKE_TOKEN_DESERIALIZE(payload, salt);

    auto signature = rawLoginToken.substr(last + 1);

    return HandShakeToken{
        .mRawHeader  = std::string(rawHeader),
        .mHeader     = std::move(header),
        .mRawPayload = std::string(rawPayload),
        .mPayload    = std::move(payload),
        .mSignature  = std::string(signature),
    };
}

} // namespace sculk::protocol::inline abi_v975
