// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/GameRuleData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void GameRuleData::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeBool(mCanBeModifiedByPlayer);
    stream.writeVariantIndex<std::uint32_t>(mData, &BinaryStream::writeUnsignedVarInt);
    std::visit(
        Overload{
            [](std::monostate) {},
            [&](bool value) { stream.writeBool(value); },
            [&](int value) { stream.writeSignedInt(value); },
            [&](float value) { stream.writeFloat(value); },
        },
        mData
    );
}

Result<> GameRuleData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readBool(mCanBeModifiedByPlayer));
    _SCULK_READ(stream.readVariantIndex<std::uint32_t>(mData, &ReadOnlyBinaryStream::readUnsignedVarInt));
    return std::visit(
        Overload{
            [](std::monostate) { return Result<>{}; },
            [&](bool& value) { return stream.readBool(value); },
            [&](int& value) { return stream.readSignedInt(value); },
            [&](float& value) { return stream.readFloat(value); },
        },
        mData
    );
}

void GameRuleData::writeLevelSettings(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeBool(mCanBeModifiedByPlayer);
    stream.writeVariantIndex<std::uint32_t>(mData, &BinaryStream::writeUnsignedVarInt);
    std::visit(
        Overload{
            [](std::monostate) {},
            [&](bool value) { stream.writeBool(value); },
            [&](int value) { stream.writeUnsignedVarInt(static_cast<std::uint32_t>(value)); },
            [&](float value) { stream.writeFloat(value); },
        },
        mData
    );
}

Result<> GameRuleData::readLevelSettings(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readBool(mCanBeModifiedByPlayer));
    _SCULK_READ(stream.readVariantIndex<std::uint32_t>(mData, &ReadOnlyBinaryStream::readUnsignedVarInt));
    return std::visit(
        Overload{
            [](std::monostate) { return Result<>{}; },
            [&](bool& value) { return stream.readBool(value); },
            [&](int& value) {
                std::uint32_t rawValue{};
                _SCULK_READ(stream.readUnsignedVarInt(rawValue));
                value = static_cast<int>(rawValue);
                return Result<>{};
            },
            [&](float& value) { return stream.readFloat(value); },
        },
        mData
    );
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
