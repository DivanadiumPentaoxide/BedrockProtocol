// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/Result.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

struct evp_cipher_ctx_st;

namespace sculk::protocol::inline abi_v944 {

struct EvpCipherCtxDeleter {
    void operator()(evp_cipher_ctx_st* ctx) const noexcept;
};

using EvpCipherCtxPtr = std::unique_ptr<evp_cipher_ctx_st, EvpCipherCtxDeleter>;

class CryptoManager {
public:
    CryptoManager() = delete;

    CryptoManager(const CryptoManager&)            = delete;
    CryptoManager& operator=(const CryptoManager&) = delete;
    CryptoManager(CryptoManager&&)                 = delete;
    CryptoManager& operator=(CryptoManager&&)      = delete;

    explicit CryptoManager(std::vector<std::byte>&& sessionKey);

    [[nodiscard]] Result<std::vector<std::byte>> encrypt(std::span<const std::byte> input);
    [[nodiscard]] Result<std::vector<std::byte>> decrypt(std::span<const std::byte> input);

    [[nodiscard]] bool verify(std::span<const std::byte> input);

public:
    static Result<std::vector<std::byte>>
    deriveSessionKey(std::span<const std::byte> salt, std::span<const std::byte> sharedSecret);

    static Result<std::vector<std::byte>>
    computeEcdhSharedSecret(std::string_view localPrivateKeyPem, std::string_view remotePublicKeyPem);

    static Result<std::vector<std::byte>> computeSessionKey(
        std::string_view           localPrivateKeyPem,
        std::string_view           remotePublicKeyPem,
        std::span<const std::byte> salt
    );

private:
    static constexpr std::size_t CHECKSUM_SIZE  = 8;
    static constexpr std::size_t AES_BLOCK_SIZE = 16;

    std::uint64_t                         mEncryptCounter{};
    std::uint64_t                         mDecryptCounter{};
    std::vector<std::byte>                mKeyBytes{};
    std::array<std::byte, AES_BLOCK_SIZE> mInitialCounterBlock{};
    EvpCipherCtxPtr                       mEncryptCtx{};
    EvpCipherCtxPtr                       mDecryptCtx{};

    [[nodiscard]] std::array<std::byte, CHECKSUM_SIZE>
    checksum(std::uint64_t counter, const std::byte* data, std::size_t dataSize) const;

    [[nodiscard]] bool                           initializeCipher(EvpCipherCtxPtr& ctx, bool encrypt) const;
    [[nodiscard]] Result<std::vector<std::byte>> ctrCrypt(EvpCipherCtxPtr& ctx, std::span<const std::byte> bytes) const;
};

} // namespace sculk::protocol::inline abi_v944