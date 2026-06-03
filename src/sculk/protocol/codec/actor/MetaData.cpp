// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/MetaData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void MetaData::write(BinaryStream& stream) const {
    stream.writeArray(mDataItems, [&](const DataItem& item) {
        stream.writeEnum(item.mId, &BinaryStream::writeUnsignedVarInt);
        stream.writeVariantIndex<std::uint32_t>(item.mData, &BinaryStream::writeUnsignedVarInt);
        std::visit(
            Overload{
                [&](uint8_t data) { stream.writeByte(data); },
                [&](short data) { stream.writeSignedShort(data); },
                [&](int data) { stream.writeVarInt(data); },
                [&](float data) { stream.writeFloat(data); },
                [&](const std::string& data) { stream.writeString(data); },
                [&](const CompoundTag& data) { data.write(stream); },
                [&](const BlockPos& data) { data.write(stream); },
                [&](int64_t data) { stream.writeVarInt64(data); },
                [&](const Vec3& data) { data.write(stream); },
            },
            item.mData
        );
    });
}

Result<> MetaData::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mDataItems, [&](DataItem& item) {
        _SCULK_READ(stream.readEnum(item.mId, &ReadOnlyBinaryStream::readUnsignedVarInt));
        _SCULK_READ(stream.readVariantIndex<std::uint32_t>(item.mData, &ReadOnlyBinaryStream::readUnsignedVarInt));
        return std::visit(
            Overload{
                [&](uint8_t& data) { return stream.readByte(data); },
                [&](short& data) { return stream.readSignedShort(data); },
                [&](int& data) { return stream.readVarInt(data); },
                [&](float& data) { return stream.readFloat(data); },
                [&](std::string& data) { return stream.readString(data); },
                [&](CompoundTag& data) { return data.read(stream); },
                [&](BlockPos& data) { return data.read(stream); },
                [&](int64_t& data) { return stream.readVarInt64(data); },
                [&](Vec3& data) { return data.read(stream); },
            },
            item.mData
        );
    });
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
