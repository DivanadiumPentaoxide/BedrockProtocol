// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/auth/PemKeyPair.hpp"
#include "../ssl/ES384.hpp"
#include "../ssl/RS256.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::ssl {

Result<PemKeyPair> randomES384KeyPair() {
    PemKeyPair keyPair{};
    if (!es384::generateES384KeyPair(keyPair.mPublicKeyPem, keyPair.mPrivateKeyPem)) {
        return error_utils::makeError("Failed to generate ES384 key pair");
    }
    return keyPair;
}

Result<PemKeyPair> randomRS256KeyPair() {
    PemKeyPair keyPair{};
    if (!rs256::generateRS256KeyPair(keyPair.mPublicKeyPem, keyPair.mPrivateKeyPem)) {
        return error_utils::makeError("Failed to generate RS256 key pair");
    }
    return keyPair;
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::ssl
