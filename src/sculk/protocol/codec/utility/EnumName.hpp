// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/utility/BinaryStream.hpp"
#include "sculk/protocol/utility/ReadOnlyBinaryStream.hpp"
#include <magic_enum/magic_enum.hpp>
#include <type_traits>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::utils {

template <typename T>
    requires std::is_enum_v<T>
[[nodiscard]] constexpr Result<>
readEnumName(ReadOnlyBinaryStream& stream, T& outValue _SCULK_SL_PARAM_DEFAULT) noexcept {
    std::string enumName{};
    if (!stream.readString(enumName _SCULK_SL_PARAM_PASS)) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(
            std::format(
                "ReadOnlyBinaryStream::readString overflowed: mReadPointer={}, size={}",
                stream.getPosition(),
                stream.size()
            )
        );
#else
        return error_utils::makeError("ReadOnlyBinaryStream::readString overflowed");
#endif
    }
    auto enumValue = magic_enum::enum_cast<T>(enumName, magic_enum::case_insensitive);
    if (!enumValue.has_value()) {
#ifdef SCULK_PROTOCOL_ENABLE_DETAIL_ERRORS
        return error_utils::makeError(
            std::format(
                "ReadOnlyBinaryStream::readEnumName received an invalid enum name '{}', this enum is bound to '{}'",
                enumName,
                magic_enum::enum_names<T>()
            )
        );
#else
        return error_utils::makeError("ReadOnlyBinaryStream::readEnumName received an invalid enum name");
#endif
    }
    outValue = *enumValue;
    return {};
}

template <typename T>
    requires std::is_enum_v<T>
constexpr void writeEnumName(BinaryStream& stream, T value) {
    auto name = std::string(magic_enum::enum_name(value));
    if (name.empty()) {
        name = std::string(magic_enum::enum_name(T{}));
    }
    for (char& c : name) {
        c = static_cast<char>(std::tolower(static_cast<std::uint8_t>(c)));
    }
    stream.writeString(name);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE::utils