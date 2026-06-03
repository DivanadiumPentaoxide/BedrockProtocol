// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/camera/CameraAimAssist.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

namespace {

inline void writePriorityMap(BinaryStream& stream, const std::map<std::string, std::int32_t>& values) {
    stream.writeUnsignedVarInt(static_cast<std::uint32_t>(values.size()));
    for (const auto& [name, priority] : values) {
        stream.writeString(name);
        stream.writeSignedInt(priority);
    }
}

inline Result<> readPriorityMap(ReadOnlyBinaryStream& stream, std::map<std::string, std::int32_t>& values) {
    values.clear();
    std::uint32_t size{};
    _SCULK_READ(stream.readUnsignedVarInt(size));
    for (std::uint32_t i = 0; i < size; ++i) {
        std::string  name{};
        std::int32_t priority{};
        _SCULK_READ(stream.readString(name));
        _SCULK_READ(stream.readSignedInt(priority));
        values.emplace(std::move(name), priority);
    }
    return {};
}

inline void writeStringMap(BinaryStream& stream, const std::map<std::string, std::string>& values) {
    stream.writeUnsignedVarInt(static_cast<std::uint32_t>(values.size()));
    for (const auto& [name, category] : values) {
        stream.writeString(name);
        stream.writeString(category);
    }
}

inline Result<> readStringMap(ReadOnlyBinaryStream& stream, std::map<std::string, std::string>& values) {
    values.clear();
    std::uint32_t size{};
    _SCULK_READ(stream.readUnsignedVarInt(size));
    for (std::uint32_t i = 0; i < size; ++i) {
        std::string name{};
        std::string category{};
        _SCULK_READ(stream.readString(name));
        _SCULK_READ(stream.readString(category));
        values.emplace(std::move(name), std::move(category));
    }
    return {};
}

} // namespace

void CameraAimAssistCategoryPriorities::write(BinaryStream& stream) const {
    writePriorityMap(stream, mEntities);
    writePriorityMap(stream, mBlocks);
    writePriorityMap(stream, mBlockTags);
    writePriorityMap(stream, mEntityTypeFamilies);
    stream.writeOptional(mEntityDefault, &BinaryStream::writeSignedInt);
    stream.writeOptional(mBlockDefault, &BinaryStream::writeSignedInt);
}

Result<> CameraAimAssistCategoryPriorities::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(readPriorityMap(stream, mEntities));
    _SCULK_READ(readPriorityMap(stream, mBlocks));
    _SCULK_READ(readPriorityMap(stream, mBlockTags));
    _SCULK_READ(readPriorityMap(stream, mEntityTypeFamilies));
    _SCULK_READ(stream.readOptional(mEntityDefault, &ReadOnlyBinaryStream::readSignedInt));
    return stream.readOptional(mBlockDefault, &ReadOnlyBinaryStream::readSignedInt);
}

void CameraAimAssistCategoryDefinition::write(BinaryStream& stream) const {
    stream.writeString(mName);
    mPriorities.write(stream);
}

Result<> CameraAimAssistCategoryDefinition::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    return mPriorities.read(stream);
}

void CameraAimAssistCategoriesDefinition::write(BinaryStream& stream) const {
    stream.writeString(mIdentifier);
    stream.writeArray(mCategory, &CameraAimAssistCategoryDefinition::write);
}

Result<> CameraAimAssistCategoriesDefinition::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mIdentifier));
    return stream.readArray(mCategory, &CameraAimAssistCategoryDefinition::read);
}

void CameraAimAssistPresetDefinition::write(BinaryStream& stream) const {
    stream.writeString(mIdentifier);
    stream.writeArray(mBlockExclusionList, &BinaryStream::writeString);
    stream.writeArray(mEntityExclusionList, &BinaryStream::writeString);
    stream.writeArray(mBlockTagExclusionList, &BinaryStream::writeString);
    stream.writeArray(mEntityTypeFamilyExclusionList, &BinaryStream::writeString);
    stream.writeArray(mLiquidTargetingList, &BinaryStream::writeString);
    writeStringMap(stream, mItemSettings);
    stream.writeOptional(mDefaultItemSettings, &BinaryStream::writeString);
    stream.writeOptional(mHandSettings, &BinaryStream::writeString);
}

Result<> CameraAimAssistPresetDefinition::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mIdentifier));
    _SCULK_READ(stream.readArray(mBlockExclusionList, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readArray(mEntityExclusionList, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readArray(mBlockTagExclusionList, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readArray(mEntityTypeFamilyExclusionList, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readArray(mLiquidTargetingList, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(readStringMap(stream, mItemSettings));
    _SCULK_READ(stream.readOptional(mDefaultItemSettings, &ReadOnlyBinaryStream::readString));
    return stream.readOptional(mHandSettings, &ReadOnlyBinaryStream::readString);
}

void CameraPresetAimAssistDefinition::write(BinaryStream& stream) const {
    stream.writeOptional(mPresetId, &BinaryStream::writeString);
    stream.writeOptional(mTargetMode, [](BinaryStream& stream, TargetMode value) {
        stream.writeEnum(value, &BinaryStream::writeByte);
    });
    stream.writeOptional(mViewAngle, &Vec2::write);
    stream.writeOptional(mDistance, &BinaryStream::writeFloat);
}

Result<> CameraPresetAimAssistDefinition::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readOptional(mPresetId, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readOptional(mTargetMode, [](ReadOnlyBinaryStream& stream, TargetMode& value) {
        return stream.readEnum(value, &ReadOnlyBinaryStream::readByte);
    }));
    _SCULK_READ(stream.readOptional(mViewAngle, &Vec2::read));
    return stream.readOptional(mDistance, &ReadOnlyBinaryStream::readFloat);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
