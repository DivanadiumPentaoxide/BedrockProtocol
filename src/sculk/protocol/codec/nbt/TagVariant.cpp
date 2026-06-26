// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/nbt/TagVariant.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void TagVariant::serialize(BinaryStream& stream) const {
    mValue.visit([&stream](const auto& val) { val.serialize(stream); });
}

Result<> TagVariant::deserialize(ReadOnlyBinaryStream& stream) {
    return mValue.visit([&stream](auto& val) { return val.deserialize(stream); });
}

TagVariant& TagVariant::emplace(TagType type) {
    switch (type) {
    case TagType::End:
        mValue.emplace<EndTag>();
        break;
    case TagType::Byte:
        mValue.emplace<ByteTag>();
        break;
    case TagType::Short:
        mValue.emplace<ShortTag>();
        break;
    case TagType::Int:
        mValue.emplace<IntTag>();
        break;
    case TagType::Long:
        mValue.emplace<LongTag>();
        break;
    case TagType::Float:
        mValue.emplace<FloatTag>();
        break;
    case TagType::Double:
        mValue.emplace<DoubleTag>();
        break;
    case TagType::ByteArray:
        mValue.emplace<ByteArrayTag>();
        break;
    case TagType::String:
        mValue.emplace<StringTag>();
        break;
    case TagType::List:
        mValue.emplace<ListTag>();
        break;
    case TagType::Compound:
        mValue.emplace<CompoundTag>();
        break;
    case TagType::IntArray:
        mValue.emplace<IntArrayTag>();
        break;
    }
    return *this;
}

TagType TagVariant::getType() const noexcept { return mValue.type(); }

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
