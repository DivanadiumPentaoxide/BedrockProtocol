// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ClientProperties.hpp"
#include "LegacyCertificateChain.hpp"
#include "LoginToken.hpp"
#include "sculk/protocol/codec/utility/deps/BinaryStream.hpp"
#include "sculk/protocol/codec/utility/deps/ReadOnlyBinaryStream.hpp"
#include "sculk/protocol/codec/utility/deps/Result.hpp"
#include "sculk/protocol/connection/AuthenticationType.hpp"
#include "sculk/protocol/connection/ConnectionRequest.hpp"

namespace sculk::protocol::inline abi_v975 {

class LoginRequest {
public:
    AuthenticationType                    mAuthenticationType{};
    std::optional<LegacyCertificateChain> mCertificateChain{};
    LoginToken                            mLoginToken{};
    ClientProperties                      mClientProperties{};

public:
    [[nodiscard]] Result<> load(const ConnectionRequest& request);

    //[[nodiscard]] ConnectionRequest toConnectionRequest() const;
};

} // namespace sculk::protocol::inline abi_v975
