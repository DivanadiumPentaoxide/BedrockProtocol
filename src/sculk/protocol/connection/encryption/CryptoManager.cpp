// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/encryption/CryptoManager.hpp"
#include "../../ssl/PemHelper.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <mutex>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace sculk::protocol::inline abi_v944 {

Result<std::vector<std::byte>>
CryptoManager::computeEcdhSharedSecret(std::string_view localPrivateKeyPem, std::string_view remotePublicKeyPem) {
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> localKey(nullptr, EVP_PKEY_free);
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> peerKey(nullptr, EVP_PKEY_free);
    std::unique_ptr<BIO, decltype(&BIO_free)>           bio(nullptr, BIO_free);

    std::string normalizedLocalPrivatePem = pem_helper::normalizePemForRead(localPrivateKeyPem, true);
    bio.reset(BIO_new_mem_buf(normalizedLocalPrivatePem.data(), static_cast<int>(normalizedLocalPrivatePem.size())));
    if (!bio) {
        return error_utils::makeError("Failed to create local private key BIO");
    }
    localKey.reset(PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr));
    if (!localKey) {
        return error_utils::makeError("Failed to load local private key");
    }

    std::string normalizedRemotePublicPem = pem_helper::normalizePemForRead(remotePublicKeyPem, false);
    bio.reset(BIO_new_mem_buf(normalizedRemotePublicPem.data(), static_cast<int>(normalizedRemotePublicPem.size())));
    if (!bio) {
        return error_utils::makeError("Failed to create peer public key BIO");
    }
    peerKey.reset(PEM_read_bio_PUBKEY(bio.get(), nullptr, nullptr, nullptr));
    if (!peerKey) {
        return error_utils::makeError("Failed to load peer public key");
    }

    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new(localKey.get(), nullptr),
        EVP_PKEY_CTX_free
    );
    if (!ctx || EVP_PKEY_derive_init(ctx.get()) <= 0) {
        return error_utils::makeError("Failed to initialize ECDH context");
    }

    if (EVP_PKEY_derive_set_peer(ctx.get(), peerKey.get()) <= 0) {
        return error_utils::makeError("Failed to set peer public key");
    }

    size_t secretLen = 0;
    if (EVP_PKEY_derive(ctx.get(), nullptr, &secretLen) <= 0) {
        return error_utils::makeError("Failed to get shared secret length");
    }

    std::vector<std::byte> sharedSecret(secretLen);
    if (EVP_PKEY_derive(ctx.get(), reinterpret_cast<std::uint8_t*>(sharedSecret.data()), &secretLen) <= 0) {
        return error_utils::makeError("Failed to compute shared secret");
    }

    return sharedSecret;
}

Result<std::vector<std::byte>>
CryptoManager::deriveSessionKey(std::span<const std::byte> token, std::span<const std::byte> sharedSecret) {
    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> mdctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
    if (!mdctx) {
        return error_utils::makeError("Failed to create digest context");
    }

    if (EVP_DigestInit_ex(mdctx.get(), EVP_sha256(), nullptr) != 1) {
        return error_utils::makeError("Failed to initialize SHA-256");
    }

    if (EVP_DigestUpdate(mdctx.get(), token.data(), token.size()) != 1) {
        return error_utils::makeError("Failed to update digest with token");
    }
    if (EVP_DigestUpdate(mdctx.get(), sharedSecret.data(), sharedSecret.size()) != 1) {
        return error_utils::makeError("Failed to update digest with shared secret");
    }

    std::vector<std::byte> sessionKey(32);
    std::uint32_t          len = 0;
    if (EVP_DigestFinal_ex(mdctx.get(), reinterpret_cast<std::uint8_t*>(sessionKey.data()), &len) != 1) {
        return error_utils::makeError("Failed to finalize digest");
    }

    return sessionKey;
}

namespace {

struct EvpMdCtxDeleter {
    void operator()(EVP_MD_CTX* ctx) const noexcept { EVP_MD_CTX_free(ctx); }
};

using EvpMdCtxPtr = std::unique_ptr<EVP_MD_CTX, EvpMdCtxDeleter>;

void writeLittleEndian64(std::array<std::byte, 8>& out, std::uint64_t value) {
    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = static_cast<std::byte>((value >> (i * 8U)) & 0xffU);
    }
}

const EVP_CIPHER* selectAesCtrCipher(std::size_t keySize) noexcept {
    switch (keySize) {
    case 16:
        return EVP_aes_128_ctr();
    case 24:
        return EVP_aes_192_ctr();
    case 32:
        return EVP_aes_256_ctr();
    default:
        return nullptr;
    }
}

} // namespace

void EvpCipherCtxDeleter::operator()(evp_cipher_ctx_st* ctx) const noexcept { EVP_CIPHER_CTX_free(ctx); }

CryptoManager::CryptoManager(std::vector<std::byte>&& keyBytes) : mKeyBytes(std::move(keyBytes)) {
    const std::size_t nonceSize = std::min<std::size_t>(mKeyBytes.size(), 12);
    if (nonceSize != 0) {
        std::copy_n(mKeyBytes.begin(), nonceSize, mInitialCounterBlock.begin());
    }
    mInitialCounterBlock[15] = std::byte{0x02};
}

std::array<std::byte, CryptoManager::CHECKSUM_SIZE>
CryptoManager::checksum(std::uint64_t counter, const std::byte* data, std::size_t dataSize) const {
    std::array<std::byte, CHECKSUM_SIZE> counterBytes{};
    writeLittleEndian64(counterBytes, counter);

    std::array<std::uint8_t, EVP_MAX_MD_SIZE> digest{};
    std::uint32_t                             digestSize = 0;
    EvpMdCtxPtr                               ctx(EVP_MD_CTX_new());
    if (!ctx || EVP_DigestInit_ex(ctx.get(), EVP_sha256(), nullptr) != 1
        || EVP_DigestUpdate(ctx.get(), counterBytes.data(), counterBytes.size()) != 1
        || (dataSize != 0 && EVP_DigestUpdate(ctx.get(), data, dataSize) != 1)
        || (!mKeyBytes.empty() && EVP_DigestUpdate(ctx.get(), mKeyBytes.data(), mKeyBytes.size()) != 1)
        || EVP_DigestFinal_ex(ctx.get(), digest.data(), &digestSize) != 1 || digestSize < CHECKSUM_SIZE) {
        return {};
    }

    std::array<std::byte, CHECKSUM_SIZE> result{};
    std::memcpy(result.data(), digest.data(), result.size());
    return result;
}

bool CryptoManager::initializeCipher(EvpCipherCtxPtr& ctx, bool encrypt) const {
    const EVP_CIPHER* cipher = selectAesCtrCipher(mKeyBytes.size());
    if (!cipher) {
        return false;
    }

    ctx.reset(EVP_CIPHER_CTX_new());
    return ctx != nullptr
        && EVP_CipherInit_ex(
               ctx.get(),
               cipher,
               nullptr,
               reinterpret_cast<const std::uint8_t*>(mKeyBytes.data()),
               reinterpret_cast<const std::uint8_t*>(mInitialCounterBlock.data()),
               encrypt ? 1 : 0
           ) == 1
        && EVP_CIPHER_CTX_set_padding(ctx.get(), 0) == 1;
}

std::vector<std::byte> CryptoManager::ctrCrypt(EvpCipherCtxPtr& ctx, std::span<const std::byte> bytes) const {
    if (bytes.empty()) {
        return {};
    }

    std::vector<std::byte> output(bytes.size());
    if (!ctx) {
        return {bytes.begin(), bytes.end()};
    }

    int        outLen = 0;
    const bool ok     = EVP_CipherUpdate(
                            ctx.get(),
                            reinterpret_cast<std::uint8_t*>(output.data()),
                            &outLen,
                            reinterpret_cast<const std::uint8_t*>(bytes.data()),
                            static_cast<int>(bytes.size())
                        )
                     == 1;

    if (!ok || static_cast<std::size_t>(outLen) != output.size()) {
        return {bytes.begin(), bytes.end()};
    }

    return output;
}

std::vector<std::byte> CryptoManager::encrypt(std::span<const std::byte> bytes) {
    if (bytes.empty()) {
        return {};
    }

    std::vector<std::byte> data{bytes.begin(), bytes.end()};
    const std::byte*       payload     = data.data();
    const std::size_t      payloadSize = data.size();
    const auto             sum         = checksum(mEncryptCounter++, payload, payloadSize);
    data.insert(data.end(), sum.begin(), sum.end());

    if (!mEncryptCtx && !initializeCipher(mEncryptCtx, true)) {
        return {bytes.begin(), bytes.end()};
    }

    return ctrCrypt(mEncryptCtx, data);
}

std::vector<std::byte> CryptoManager::decrypt(std::span<const std::byte> bytes) {
    if (bytes.empty()) {
        return {};
    }
    if (bytes.size() < CHECKSUM_SIZE) {
        return {};
    }

    if (!mDecryptCtx && !initializeCipher(mDecryptCtx, false)) {
        return {bytes.begin(), bytes.end()};
    }

    auto clear = ctrCrypt(mDecryptCtx, bytes);
    if (!verify(clear)) {
        return {};
    }

    clear.resize(clear.size() - CHECKSUM_SIZE);
    return clear;
}

bool CryptoManager::verify(std::span<const std::byte> bytes) {
    if (bytes.size() < CHECKSUM_SIZE) {
        return false;
    }

    const std::size_t payloadSize = bytes.size() - CHECKSUM_SIZE;
    const auto        expected    = checksum(mDecryptCounter++, bytes.data(), payloadSize);
    return std::equal(expected.begin(), expected.end(), bytes.begin() + static_cast<std::ptrdiff_t>(payloadSize));
}

} // namespace sculk::protocol::inline abi_v944