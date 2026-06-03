// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/recipe/Recipes.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void RecipeUnlockingRequirement::write(BinaryStream& stream) const {
    stream.writeEnum(mUnlockingContext, &BinaryStream::writeByte);
    if (mUnlockingContext == UnlockingContext::None) {
        stream.writeArray(mUnlockingIngredients, &RecipeIngredient::write);
    }
}

Result<> RecipeUnlockingRequirement::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mUnlockingContext, &ReadOnlyBinaryStream::readByte));
    if (mUnlockingContext == UnlockingContext::None) {
        return stream.readArray(mUnlockingIngredients, &RecipeIngredient::read);
    }
    return {};
}

void ShapelessRecipe::write(BinaryStream& stream) const {
    stream.writeString(mRecipeUniqueId);
    stream.writeArray(mIngredientList, &RecipeIngredient::write);
    stream.writeArray(mProductionList, &NetworkItemInstanceDescriptor::write);
    mRecipeId.write(stream);
    stream.writeString(mRecipeTag);
    stream.writeVarInt(mPriority);
    mUnlockingRequirement.write(stream);
    stream.writeUnsignedVarInt(mNetId);
}

Result<> ShapelessRecipe::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mRecipeUniqueId));
    _SCULK_READ(stream.readArray(mIngredientList, &RecipeIngredient::read));
    _SCULK_READ(stream.readArray(mProductionList, &NetworkItemInstanceDescriptor::read));
    _SCULK_READ(mRecipeId.read(stream));
    _SCULK_READ(stream.readString(mRecipeTag));
    _SCULK_READ(stream.readVarInt(mPriority));
    _SCULK_READ(mUnlockingRequirement.read(stream));
    return stream.readUnsignedVarInt(mNetId);
}

void ShapedRecipe::write(BinaryStream& stream) const {
    stream.writeString(mRecipeUniqueId);
    stream.writeVarInt(mGridWidth);
    stream.writeVarInt(mGridHeight);
    for (const RecipeIngredient& ingredient : mIngredientList) {
        ingredient.write(stream);
    }
    stream.writeArray(mProductionList, &NetworkItemInstanceDescriptor::write);
    mRecipeId.write(stream);
    stream.writeString(mRecipeTag);
    stream.writeVarInt(mPriority);
    stream.writeBool(mAssumeSymmetry);
    mUnlockingRequirement.write(stream);
    stream.writeUnsignedVarInt(mNetId);
}

Result<> ShapedRecipe::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mRecipeUniqueId));
    _SCULK_READ(stream.readVarInt(mGridWidth));
    _SCULK_READ(stream.readVarInt(mGridHeight));
    mIngredientList.resize(static_cast<std::size_t>(mGridWidth * mGridHeight));
    for (RecipeIngredient& ingredient : mIngredientList) {
        _SCULK_READ(ingredient.read(stream));
    }
    _SCULK_READ(stream.readArray(mProductionList, &NetworkItemInstanceDescriptor::read));
    _SCULK_READ(mRecipeId.read(stream));
    _SCULK_READ(stream.readString(mRecipeTag));
    _SCULK_READ(stream.readVarInt(mPriority));
    _SCULK_READ(stream.readBool(mAssumeSymmetry));
    _SCULK_READ(mUnlockingRequirement.read(stream));
    return stream.readUnsignedVarInt(mNetId);
}

void FurnaceRecipe::write(BinaryStream&) const {}

Result<> FurnaceRecipe::read(ReadOnlyBinaryStream&) { return {}; }

void FurnaceAuxRecipe::write(BinaryStream&) const {}

Result<> FurnaceAuxRecipe::read(ReadOnlyBinaryStream&) { return {}; }

void MultiRecipe::write(BinaryStream& stream) const {
    mMultiRecipe.write(stream);
    stream.writeUnsignedVarInt(mNetId);
}

Result<> MultiRecipe::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mMultiRecipe.read(stream));
    return stream.readUnsignedVarInt(mNetId);
}

void UserDataShapelessRecipe::write(BinaryStream& stream) const {
    stream.writeString(mRecipeUniqueId);
    stream.writeArray(mIngredientList, &RecipeIngredient::write);
    stream.writeArray(mProductionList, &NetworkItemInstanceDescriptor::write);
    mRecipeId.write(stream);
    stream.writeString(mRecipeTag);
    stream.writeVarInt(mPriority);
    mUnlockingRequirement.write(stream);
    stream.writeUnsignedVarInt(mNetId);
}

Result<> UserDataShapelessRecipe::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mRecipeUniqueId));
    _SCULK_READ(stream.readArray(mIngredientList, &RecipeIngredient::read));
    _SCULK_READ(stream.readArray(mProductionList, &NetworkItemInstanceDescriptor::read));
    _SCULK_READ(mRecipeId.read(stream));
    _SCULK_READ(stream.readString(mRecipeTag));
    _SCULK_READ(stream.readVarInt(mPriority));
    _SCULK_READ(mUnlockingRequirement.read(stream));
    return stream.readUnsignedVarInt(mNetId);
}

void ShapelessChemistryRecipe::write(BinaryStream& stream) const {
    stream.writeString(mRecipeUniqueId);
    stream.writeArray(mIngredientList, &RecipeIngredient::write);
    stream.writeArray(mProductionList, &NetworkItemInstanceDescriptor::write);
    mRecipeId.write(stream);
    stream.writeString(mRecipeTag);
    stream.writeVarInt(mPriority);
    stream.writeUnsignedVarInt(mNetId);
}

Result<> ShapelessChemistryRecipe::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mRecipeUniqueId));
    _SCULK_READ(stream.readArray(mIngredientList, &RecipeIngredient::read));
    _SCULK_READ(stream.readArray(mProductionList, &NetworkItemInstanceDescriptor::read));
    _SCULK_READ(mRecipeId.read(stream));
    _SCULK_READ(stream.readString(mRecipeTag));
    _SCULK_READ(stream.readVarInt(mPriority));
    return stream.readUnsignedVarInt(mNetId);
}

void ShapedChemistryRecipe::write(BinaryStream& stream) const { mChemistryRecipe.write(stream); }

Result<> ShapedChemistryRecipe::read(ReadOnlyBinaryStream& stream) { return mChemistryRecipe.read(stream); }

void SmithingTransformRecipe::write(BinaryStream& stream) const {
    stream.writeString(mRecipeUniqueId);
    mTemplate.write(stream);
    mBase.write(stream);
    mAddition.write(stream);
    mResult.write(stream);
    stream.writeString(mRecipeTag);
    stream.writeUnsignedVarInt(mNetId);
}

Result<> SmithingTransformRecipe::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mRecipeUniqueId));
    _SCULK_READ(mTemplate.read(stream));
    _SCULK_READ(mBase.read(stream));
    _SCULK_READ(mAddition.read(stream));
    _SCULK_READ(mResult.read(stream));
    _SCULK_READ(stream.readString(mRecipeTag));
    return stream.readUnsignedVarInt(mNetId);
}

void SmithingTrimRecipe::write(BinaryStream& stream) const {
    stream.writeString(mRecipeUniqueId);
    mTemplate.write(stream);
    mBase.write(stream);
    mAddition.write(stream);
    stream.writeString(mRecipeTag);
    stream.writeUnsignedVarInt(mNetId);
}

Result<> SmithingTrimRecipe::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mRecipeUniqueId));
    _SCULK_READ(mTemplate.read(stream));
    _SCULK_READ(mBase.read(stream));
    _SCULK_READ(mAddition.read(stream));
    _SCULK_READ(stream.readString(mRecipeTag));
    return stream.readUnsignedVarInt(mNetId);
}

void CraftingDataEntry::write(BinaryStream& stream) const {
    stream.writeVariantIndex<std::int32_t>(mRecipe, &BinaryStream::writeVarInt);
    std::visit([&](auto&& recipe) { recipe.write(stream); }, mRecipe);
}

Result<> CraftingDataEntry::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVariantIndex<std::int32_t>(mRecipe, &ReadOnlyBinaryStream::readVarInt));
    return std::visit([&](auto&& recipe) { return recipe.read(stream); }, mRecipe);
}

void PotionMixDataEntry::write(BinaryStream& stream) const {
    stream.writeVarInt(mFromPotionId);
    stream.writeVarInt(mFromPotionAux);
    stream.writeVarInt(mReagentItemId);
    stream.writeVarInt(mReagentItemAux);
    stream.writeVarInt(mToPotionId);
    stream.writeVarInt(mToPotionAux);
}

Result<> PotionMixDataEntry::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mFromPotionId));
    _SCULK_READ(stream.readVarInt(mFromPotionAux));
    _SCULK_READ(stream.readVarInt(mReagentItemId));
    _SCULK_READ(stream.readVarInt(mReagentItemAux));
    _SCULK_READ(stream.readVarInt(mToPotionId));
    return stream.readVarInt(mToPotionAux);
}

void ContainerMixDataEntry::write(BinaryStream& stream) const {
    stream.writeVarInt(mInputItemId);
    stream.writeVarInt(mReagentItemId);
    stream.writeVarInt(mToItemId);
}

Result<> ContainerMixDataEntry::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mInputItemId));
    _SCULK_READ(stream.readVarInt(mReagentItemId));
    return stream.readVarInt(mToItemId);
}

void MaterialReducerDataEntry::ItemInfo::write(BinaryStream& stream) const {
    stream.writeVarInt(mItemId);
    stream.writeVarInt(mItemCount);
}

Result<> MaterialReducerDataEntry::ItemInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mItemId));
    return stream.readVarInt(mItemCount);
}

void MaterialReducerDataEntry::write(BinaryStream& stream) const {
    stream.writeVarInt(mInput);
    stream.writeArray(mItems, &ItemInfo::write);
}

Result<> MaterialReducerDataEntry::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt(mInput));
    return stream.readArray(mItems, &ItemInfo::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
