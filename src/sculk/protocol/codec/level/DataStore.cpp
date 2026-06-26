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
    stream.writeVariant(
        mData,
        Overload{
            [&](double value) { stream.writeDouble(value); },
            [&](bool value) { stream.writeBool(value); },
            [&](const std::string& value) { stream.writeString(value); },
        }
    );
    stream.writeUnsignedInt(mPropertyUpdateCount);
    stream.writeUnsignedInt(mPathUpdateCount);
}

Result<> DataStoreUpdate::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readString(mProperty));
    _SCULK_READ(stream.readString(mPath));
    _SCULK_READ(stream.readVariant(
        mData,
        Overload{
            [&](double& value) { return stream.readDouble(value); },
            [&](bool& value) { return stream.readBool(value); },
            [&](std::string& value) { return stream.readString(value); },
        }
    ));
    _SCULK_READ(stream.readUnsignedInt(mPropertyUpdateCount));
    return stream.readUnsignedInt(mPathUpdateCount);
}

void DynamicValue::write(BinaryStream& stream) const {
    stream.writeVariant(
        mValue,
        &BinaryStream::writeUnsignedInt,
        Overload{
            [&](std::monostate) {},
            [&](bool value) { stream.writeBool(value); },
            [&](std::int64_t value) { stream.writeSignedInt64(value); },
            [&](double value) { stream.writeDouble(value); },
            [&](const std::string& value) { stream.writeString(value); },
            [&](const std::vector<DynamicValue>& value) { stream.writeArray(value, &DynamicValue::write); },
            [&](const std::map<std::string, DynamicValue>& value) {
                stream.writeMap(value, [](BinaryStream& stream, const std::string& key, const DynamicValue& val) {
                    stream.writeString(key);
                    val.write(stream);
                });
            },
        }
    );
}

Result<> DynamicValue::read(ReadOnlyBinaryStream& stream) {
    return stream.readVariant(
        mValue,
        &ReadOnlyBinaryStream::readUnsignedInt,
        Overload{
            [&](std::monostate) { return Result<>{}; },
            [&](bool& value) { return stream.readBool(value); },
            [&](std::int64_t& value) { return stream.readSignedInt64(value); },
            [&](double& value) { return stream.readDouble(value); },
            [&](std::string& value) { return stream.readString(value); },
            [&](std::vector<DynamicValue>& value) {
                return stream.readArray(value, [](ReadOnlyBinaryStream& stream, DynamicValue& item) {
                    return item.read(stream);
                });
            },
            [&](std::map<std::string, DynamicValue>& value) {
                return stream.readMap(value, [](ReadOnlyBinaryStream& stream, std::string& key, DynamicValue& val) {
                    _SCULK_READ(stream.readString(key));
                    return val.read(stream);
                });
            }
        }
    );
}

void DataStoreChange::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeString(mProperty);
    stream.writeUnsignedInt(mUpdateCount);
    mValue.write(stream);
}

Result<> DataStoreChange::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readString(mProperty));
    _SCULK_READ(stream.readUnsignedInt(mUpdateCount));
    return mValue.read(stream);
}

void DataStoreRemoval::write(BinaryStream& stream) const { stream.writeString(mName); }

Result<> DataStoreRemoval::read(ReadOnlyBinaryStream& stream) { return stream.readString(mName); }

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
