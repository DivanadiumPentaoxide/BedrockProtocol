// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/DataStore.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void DataStoreUpdate::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeString(mProperty);
    stream.writeString(mPath);
    stream.writeVariantIndex<std::uint32_t>(mData, &BinaryStream::writeUnsignedVarInt);
    std::visit(
        Overload{
            [&](double value) { stream.writeDouble(value); },
            [&](bool value) { stream.writeBool(value); },
            [&](const std::string& value) { stream.writeString(value); },
        },
        mData
    );
    stream.writeUnsignedInt(mPropertyUpdateCount);
    stream.writeUnsignedInt(mPathUpdateCount);
}

Result<> DataStoreUpdate::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readString(mProperty));
    _SCULK_READ(stream.readString(mPath));
    _SCULK_READ(stream.readVariantIndex<std::uint32_t>(mData, &ReadOnlyBinaryStream::readUnsignedVarInt));
    _SCULK_READ(
        std::visit(
            Overload{
                [&](double& value) { return stream.readDouble(value); },
                [&](bool& value) { return stream.readBool(value); },
                [&](std::string& value) { return stream.readString(value); },
            },
            mData
        )
    );
    _SCULK_READ(stream.readUnsignedInt(mPropertyUpdateCount));
    return stream.readUnsignedInt(mPathUpdateCount);
}

void DataStoreChange::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeString(mProperty);
    stream.writeUnsignedInt(mUpdateCount);
    stream.writeVariantIndex<std::uint32_t>(mData, &BinaryStream::writeUnsignedVarInt);
    std::visit(
        Overload{
            [&](double value) { stream.writeDouble(value); },
            [&](bool value) { stream.writeBool(value); },
            [&](const std::string& value) { stream.writeString(value); },
        },
        mData
    );
}

Result<> DataStoreChange::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readString(mProperty));
    _SCULK_READ(stream.readUnsignedInt(mUpdateCount));
    _SCULK_READ(stream.readVariantIndex<std::uint32_t>(mData, &ReadOnlyBinaryStream::readUnsignedVarInt));
    return std::visit(
        Overload{
            [&](double& value) { return stream.readDouble(value); },
            [&](bool& value) { return stream.readBool(value); },
            [&](std::string& value) { return stream.readString(value); },
        },
        mData
    );
}

void DataStoreRemoval::write(BinaryStream& stream) const { stream.writeString(mName); }

Result<> DataStoreRemoval::read(ReadOnlyBinaryStream& stream) { return stream.readString(mName); }

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
