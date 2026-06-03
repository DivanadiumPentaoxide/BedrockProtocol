// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/connection/compression/Snappy.hpp"
#include "sculk/protocol/utility/Result.hpp"
#include <snappy.h>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::compression::snappy {

std::vector<std::byte> compress(const std::vector<std::byte>& data) {
    if (data.empty()) {
        return {};
    }

    const std::size_t maxCompressedLength = ::snappy::MaxCompressedLength(data.size());

    std::vector<std::byte> output(maxCompressedLength);
    std::size_t            compressedLength = maxCompressedLength;
    ::snappy::RawCompress(
        reinterpret_cast<const char*>(data.data()),
        data.size(),
        reinterpret_cast<char*>(output.data()),
        &compressedLength
    );

    output.resize(compressedLength);
    return output;
}

Result<std::vector<std::byte>> decompress(const std::vector<std::byte>& data) {
    if (data.empty()) {
        return error_utils::makeError("snappy decompress failed: input is empty");
    }

    std::size_t uncompressedLength = 0;
    if (!::snappy::GetUncompressedLength(
            reinterpret_cast<const char*>(data.data()),
            data.size(),
            &uncompressedLength
        )) {
        return error_utils::makeError("snappy decompress failed: invalid compressed payload");
    }

    constexpr std::size_t kMaxOutputSize = 64ull * 1024ull * 1024ull;
    if (uncompressedLength > kMaxOutputSize) {
        return error_utils::makeError("snappy decompress failed: output exceeds configured limit");
    }

    std::vector<std::byte> output(uncompressedLength);
    if (uncompressedLength == 0) {
        return output;
    }

    if (!::snappy::RawUncompress(
            reinterpret_cast<const char*>(data.data()),
            data.size(),
            reinterpret_cast<char*>(output.data())
        )) {
        return error_utils::makeError("snappy decompress failed");
    }

    return output;
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::compression::snappy