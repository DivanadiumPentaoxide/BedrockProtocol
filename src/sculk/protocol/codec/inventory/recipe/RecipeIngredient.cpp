// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/recipe/RecipeIngredient.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void RecipeIngredient::write(BinaryStream& stream) const {
    stream.writeVariantIndex<std::uint8_t>(mDescriptor, &BinaryStream::writeByte);
    std::visit(
        Overload{
            [&](const InternalItemDescriptor& descriptor) {
                stream.writeSignedShort(descriptor.mId);
                if (descriptor.mId != 0) {
                    stream.writeSignedShort(descriptor.mAux);
                }
            },
            [&](const MolangDescriptor& descriptor) {
                stream.writeString(descriptor.mMolangFullName);
                stream.writeByte(descriptor.mMolangVersion);
            },
            [&](const ItemTagDescriptor& descriptor) { stream.writeString(descriptor.mItemTag); },
            [&](const DeferredDescriptor& descriptor) {
                stream.writeString(descriptor.mDeferredFullName);
                stream.writeUnsignedShort(descriptor.mAux);
            },
            [&](const ComplexAliasDescriptor& descriptor) { stream.writeString(descriptor.mName); },
            [&](const std::monostate&) {}
        },
        mDescriptor
    );
    stream.writeVarInt(mStackSize);
}

Result<> RecipeIngredient::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVariantIndex<std::uint8_t>(mDescriptor, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(
        std::visit(
            Overload{
                [&](InternalItemDescriptor& descriptor) {
                    _SCULK_READ(stream.readSignedShort(descriptor.mId));
                    if (descriptor.mId != 0) {
                        _SCULK_READ(stream.readSignedShort(descriptor.mAux));
                    }
                    return Result<>{};
                },
                [&](MolangDescriptor& descriptor) {
                    _SCULK_READ(stream.readString(descriptor.mMolangFullName));
                    return stream.readByte(descriptor.mMolangVersion);
                },
                [&](ItemTagDescriptor& descriptor) { return stream.readString(descriptor.mItemTag); },
                [&](DeferredDescriptor& descriptor) {
                    _SCULK_READ(stream.readString(descriptor.mDeferredFullName));
                    return stream.readUnsignedShort(descriptor.mAux);
                },
                [&](ComplexAliasDescriptor& descriptor) { return stream.readString(descriptor.mName); },
                [&](std::monostate&) { return Result<>{}; }
            },
            mDescriptor
        )
    );
    return stream.readVarInt(mStackSize);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
