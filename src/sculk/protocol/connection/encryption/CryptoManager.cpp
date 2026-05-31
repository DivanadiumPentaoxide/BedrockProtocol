// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/encryption/CryptoManager.hpp"
#include <algorithm>
#include <cstring>
#include <memory>
#include <mutex>
#include <openssl/evp.h>
#include <utility>
#include <vector>

namespace sculk::protocol::inline abi_v944 {

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

    std::array<unsigned char, EVP_MAX_MD_SIZE> digest{};
    unsigned int                               digestSize = 0;
    EvpMdCtxPtr                                ctx(EVP_MD_CTX_new());
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
               reinterpret_cast<const unsigned char*>(mKeyBytes.data()),
               reinterpret_cast<const unsigned char*>(mInitialCounterBlock.data()),
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
                            reinterpret_cast<unsigned char*>(output.data()),
                            &outLen,
                            reinterpret_cast<const unsigned char*>(bytes.data()),
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