// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/nbt/ListTag.hpp"
#include "sculk/protocol/codec/nbt/TagVariant.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ListTag::serialize(BinaryStream& stream) const {
    stream.writeEnum(mType, &BinaryStream::writeByte);
    stream.writeVarInt(static_cast<std::int32_t>(mValue.size()));
    for (const auto& val : mValue) {
        val.serialize(stream);
    }
}

Result<> ListTag::deserialize(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mType, &ReadOnlyBinaryStream::readByte));
    std::int32_t size{};
    _SCULK_READ(stream.readVarInt(size));
    mValue.resize(size);
    for (std::int32_t i = 0; i < size; ++i) {
        mValue[i].emplace(mType);
        _SCULK_READ(mValue[i].deserialize(stream));
    }
    return {};
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
