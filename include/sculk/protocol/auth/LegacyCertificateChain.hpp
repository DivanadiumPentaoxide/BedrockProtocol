// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "AuthenticationKeyManager.hpp"
#include "PemKeyPair.hpp"
#include "sculk/protocol/utility/Result.hpp"
#include <chrono>
#include <format>
#include <optional>
#include <string>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class Certificate {
public:
    struct Header {
        std::string                alg;
        std::string                x5u;
        std::optional<std::string> x5t;
    };

    struct ExtraData {
        std::string identity{};
        std::string displayName{};
        std::string XUID{};
        std::string titleId{};
        std::string sandBoxId{};
    };

    struct Payload {
        std::int64_t                nbf{};
        std::int64_t                exp{};
        std::string                 identityPublicKey{};
        std::optional<bool>         certificateAuthority{};
        std::optional<std::int64_t> randomNonce{};
        std::optional<std::string>  iss{};
        std::optional<std::int64_t> iat{};
        std::optional<ExtraData>    extraData{};
    };

public:
    std::string mRawHeader{};
    Header      mHeader{};
    std::string mRawPayload{};
    Payload     mPayload{};
    std::string mSignature{};

public:
    [[nodiscard]] bool checkTimeValidity(std::chrono::seconds leeway, std::chrono::system_clock::time_point now) const;

    [[nodiscard]] bool checkIssuer(std::string_view expectedIssuer) const;

    [[nodiscard]] std::string toString() const { return std::format("{}.{}.{}", mRawHeader, mRawPayload, mSignature); }

    [[nodiscard]] bool verify(std::string_view publicKeyPem) const;

    [[nodiscard]] bool sign(std::string_view privateKeyPem, std::chrono::system_clock::time_point now);

public:
    [[nodiscard]] static Result<Certificate> fromString(std::string_view certificateStr);
};

class LegacyCertificateChain {
public:
    std::optional<Certificate> mClientCertificate{};
    std::optional<Certificate> mMojangCertificate{};
    Certificate                mLoginCertificate{};

public:
    [[nodiscard]] std::string getClientPublicKey() const {
        return mClientCertificate ? mClientCertificate->mPayload.identityPublicKey
                                  : mLoginCertificate.mPayload.identityPublicKey;
    }

    [[nodiscard]] Result<> verifyOnline(std::chrono::seconds leeway) const;

    [[nodiscard]] Result<> verifySelfSigned(std::chrono::seconds leeway) const;

    [[nodiscard]] Result<> selfSign(const PemKeyPair& clientKeyPair);

    [[nodiscard]] std::string toString() const;

public:
    [[nodiscard]] static Result<LegacyCertificateChain> fromString(std::string_view certificateChainJsonStr);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE