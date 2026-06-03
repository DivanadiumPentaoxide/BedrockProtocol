// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/item/ItemStackRequest.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ItemStackRequestSlotInfo::write(BinaryStream& stream) const {
    mFullContainerName.write(stream);
    stream.writeByte(mSlot);
    stream.writeVarInt(mNetId);
}

Result<> ItemStackRequestSlotInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mFullContainerName.read(stream));
    _SCULK_READ(stream.readByte(mSlot));
    return stream.readVarInt(mNetId);
}

void ItemStackRequestAction::write(BinaryStream& stream) const {
    stream.writeEnum(mActionType, &BinaryStream::writeByte);
    std::visit(
        Overload{
            [&](const TransferBase& data) {
                stream.writeByte(data.mAmount);
                data.mSource.write(stream);
                data.mDestination.write(stream);
            },
            [&](const Swap& data) {
                data.mSource.write(stream);
                data.mDestination.write(stream);
            },
            [&](const Drop& data) {
                stream.writeByte(data.mAmount);
                data.mSource.write(stream);
                stream.writeBool(data.mRandomly);
            },
            [&](const DestroyConsume& data) {
                stream.writeByte(data.mAmount);
                data.mSource.write(stream);
            },
            [&](const Create& data) { stream.writeByte(data.mResultsIndex); },
            [&](const BeaconPayment& data) {
                stream.writeVarInt(data.mPrimaryEffectId);
                stream.writeVarInt(data.mSecondaryEffectId);
            },
            [&](const MineBlock& data) {
                stream.writeVarInt(data.mSlot);
                stream.writeVarInt(data.mPredictedDurability);
                if (data.mPreValidationStatus == MineBlock::PreValidationStatus::Valid && data.mItemStackNetId != 0) {
                    stream.writeVarInt(data.mItemStackNetId);
                }
            },
            [&](const CraftRecipe& data) {
                stream.writeUnsignedVarInt(data.mRecipeNetworkIdOrCreativeId);
                stream.writeUnsignedVarInt(data.mTimesCrafted);
            },
            [&](const CraftRecipeAuto& data) {
                stream.writeUnsignedVarInt(data.mRecipeNetworkId);
                stream.writeByte(data.mNumberOfRequestedCrafts);
                stream.writeByte(data.mTimesCrafted);
                stream.writeByte(static_cast<std::uint8_t>(data.mIngredients.size()));
                for (const RecipeIngredient& ingredient : data.mIngredients) {
                    ingredient.write(stream);
                }
            },
            [&](const CraftRecipeOptional& data) {
                stream.writeUnsignedVarInt(data.mRecipeNetId);
                stream.writeUnsignedInt(data.mFilteredStringIndex);
            },
            [&](const CraftGrindStone& data) {
                stream.writeUnsignedVarInt(data.mItemStackNetId);
                stream.writeByte(data.mTimesCrafted);
                stream.writeVarInt(data.mRepairCost);
            },
            [&](const CraftLoom& data) {
                stream.writeString(data.mPatternNameId);
                stream.writeByte(data.mTimesCrafted);
            },
            [&](const CraftResult& data) {
                stream.writeUnsignedVarInt(static_cast<std::uint32_t>(data.mCraftResults.size()));
                for (const NetworkItemInstanceDescriptor& result : data.mCraftResults) {
                    result.write(stream);
                }
                stream.writeByte(data.mTimesCrafted);
            },
            [&](const OnlyType&) {}
        },
        mVariant
    );
}

Result<> ItemStackRequestAction::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mActionType, &ReadOnlyBinaryStream::readByte));
    switch (mActionType) {
    case Type::Take:
    case Type::Place: {
        TransferBase data{};
        _SCULK_READ(stream.readByte(data.mAmount));
        _SCULK_READ(data.mSource.read(stream));
        _SCULK_READ(data.mDestination.read(stream));
        mVariant = std::move(data);
        return {};
    }
    case Type::Swap: {
        Swap data{};
        _SCULK_READ(data.mSource.read(stream));
        _SCULK_READ(data.mDestination.read(stream));
        mVariant = std::move(data);
        return {};
    }
    case Type::Drop: {
        Drop data{};
        _SCULK_READ(stream.readByte(data.mAmount));
        _SCULK_READ(data.mSource.read(stream));
        _SCULK_READ(stream.readBool(data.mRandomly));
        mVariant = std::move(data);
        return {};
    }
    case Type::Destroy:
    case Type::Consume: {
        DestroyConsume data{};
        _SCULK_READ(stream.readByte(data.mAmount));
        _SCULK_READ(data.mSource.read(stream));
        mVariant = std::move(data);
        return {};
    }
    case Type::Create: {
        Create data{};
        _SCULK_READ(stream.readByte(data.mResultsIndex));
        mVariant = std::move(data);
        return {};
    }
    case Type::BeaconPayment: {
        BeaconPayment data{};
        _SCULK_READ(stream.readVarInt(data.mPrimaryEffectId));
        _SCULK_READ(stream.readVarInt(data.mSecondaryEffectId));
        mVariant = std::move(data);
        return {};
    }
    case Type::MineBlock: {
        MineBlock data{};
        _SCULK_READ(stream.readVarInt(data.mSlot));
        _SCULK_READ(stream.readVarInt(data.mPredictedDurability));
        _SCULK_READ(stream.readVarInt(data.mItemStackNetId));
        data.mPreValidationStatus =
            data.mItemStackNetId == 0 ? MineBlock::PreValidationStatus::Invalid : MineBlock::PreValidationStatus::Valid;
        mVariant = std::move(data);
        return {};
    }
    case Type::CraftRecipe:
    case Type::CraftCreative: {
        CraftRecipe data{};
        _SCULK_READ(stream.readUnsignedVarInt(data.mRecipeNetworkIdOrCreativeId));
        _SCULK_READ(stream.readUnsignedVarInt(data.mTimesCrafted));
        mVariant = std::move(data);
        return {};
    }
    case Type::CraftRecipeAuto: {
        CraftRecipeAuto data{};
        _SCULK_READ(stream.readUnsignedVarInt(data.mRecipeNetworkId));
        _SCULK_READ(stream.readByte(data.mNumberOfRequestedCrafts));
        _SCULK_READ(stream.readByte(data.mTimesCrafted));
        std::uint8_t ingredientCount{};
        _SCULK_READ(stream.readByte(ingredientCount));
        data.mIngredients.resize(ingredientCount);
        for (RecipeIngredient& ingredient : data.mIngredients) {
            _SCULK_READ(ingredient.read(stream));
        }
        mVariant = std::move(data);
        return {};
    }
    case Type::CraftRecipeOptional: {
        CraftRecipeOptional data{};
        _SCULK_READ(stream.readUnsignedVarInt(data.mRecipeNetId));
        _SCULK_READ(stream.readUnsignedInt(data.mFilteredStringIndex));
        mVariant = std::move(data);
        return {};
    }
    case Type::CraftGrindStone: {
        CraftGrindStone data{};
        _SCULK_READ(stream.readUnsignedVarInt(data.mItemStackNetId));
        _SCULK_READ(stream.readByte(data.mTimesCrafted));
        _SCULK_READ(stream.readVarInt(data.mRepairCost));
        mVariant = std::move(data);
        return {};
    }
    case Type::CraftLoom: {
        CraftLoom data{};
        _SCULK_READ(stream.readString(data.mPatternNameId));
        _SCULK_READ(stream.readByte(data.mTimesCrafted));
        mVariant = std::move(data);
        return {};
    }
    case Type::CraftResults: {
        CraftResult   data{};
        std::uint32_t resultCount{};
        _SCULK_READ(stream.readUnsignedVarInt(resultCount));
        data.mCraftResults.resize(resultCount);
        for (NetworkItemInstanceDescriptor& result : data.mCraftResults) {
            _SCULK_READ(result.read(stream));
        }
        _SCULK_READ(stream.readByte(data.mTimesCrafted));
        mVariant = std::move(data);
        return {};
    }
    default:
        mVariant = OnlyType{};
        return {};
    }
}

void ItemStackRequestData::write(BinaryStream& stream) const {
    stream.writeVarInt(mClientRequestId);
    stream.writeArray(mActions, &ItemStackRequestAction::write);
    stream.writeArray(mStringsToFilter, &BinaryStream::writeString);
    stream.writeSignedInt(mStringsToFilterOrigin);
}

Result<> ItemStackRequestData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mClientRequestId));
    _SCULK_READ(stream.readArray(mActions, &ItemStackRequestAction::read));
    _SCULK_READ(stream.readArray(mStringsToFilter, &ReadOnlyBinaryStream::readString));
    return stream.readSignedInt(mStringsToFilterOrigin);
}

void ItemStackRequest::write(BinaryStream& stream) const { stream.writeArray(mRequests, &ItemStackRequestData::write); }

Result<> ItemStackRequest::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mRequests, &ItemStackRequestData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
