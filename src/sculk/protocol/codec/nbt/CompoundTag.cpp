// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/nbt/TagVariant.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void CompoundTag::serialize(BinaryStream& stream) const {
    for (const auto& [key, value] : mValue) {
        TagType type = value.getType();
        if (type != TagType::End) {
            stream.writeEnum(type, &BinaryStream::writeByte);
            stream.writeString(key);
            value.serialize(stream);
        }
    }
    stream.writeByte(0); // End tag
}

Result<> CompoundTag::deserialize(ReadOnlyBinaryStream& stream) {
    while (true) {
        TagType tagType{};
        _SCULK_READ(stream.readEnum(tagType, &ReadOnlyBinaryStream::readByte));
        if (tagType == TagType::End) {
            break;
        }
        std::string key{};
        _SCULK_READ(stream.readString(key));
        TagVariant value{};
        value.emplace(tagType);
        _SCULK_READ(value.deserialize(stream));
        mValue.emplace(std::move(key), std::move(value));
    }
    return {};
}

void CompoundTag::write(BinaryStream& stream) const {
    stream.writeEnum(TagType::Compound, &BinaryStream::writeByte);
    stream.writeByte(0); // Root name (empty string)
    serialize(stream);
}

[[nodiscard]] Result<> CompoundTag::read(ReadOnlyBinaryStream& stream) {
    TagType rootType{};
    _SCULK_READ(stream.readEnum(rootType, &ReadOnlyBinaryStream::readByte));
    if (rootType != TagType::Compound) {
        return error_utils::makeError("CompoundTag::read invalid root tag type");
    }
    _SCULK_READ(stream.ignoreBytes(1)); // Ignore root name (empty string)
    return deserialize(stream);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
