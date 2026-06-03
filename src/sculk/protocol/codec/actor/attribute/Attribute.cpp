// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/Attribute.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void Attribute::Modifier::write(BinaryStream& stream) const {
    stream.writeString(mId);
    stream.writeString(mName);
    stream.writeFloat(mAmount);
    stream.writeSignedInt(mOperation);
    stream.writeSignedInt(mOperand);
    stream.writeBool(mIsSerializable);
}

Result<> Attribute::Modifier::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mId));
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readFloat(mAmount));
    _SCULK_READ(stream.readSignedInt(mOperation));
    _SCULK_READ(stream.readSignedInt(mOperand));
    return stream.readBool(mIsSerializable);
}

void Attribute::write(BinaryStream& stream) const {
    stream.writeFloat(mMinValue);
    stream.writeFloat(mMaxValue);
    stream.writeFloat(mCurrentValue);
    stream.writeFloat(mDefaultMinValue);
    stream.writeFloat(mDefaultMaxValue);
    stream.writeFloat(mDefaultValue);
    stream.writeString(mName);
    stream.writeArray(mModifiers, &Attribute::Modifier::write);
}

Result<> Attribute::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readFloat(mMinValue));
    _SCULK_READ(stream.readFloat(mMaxValue));
    _SCULK_READ(stream.readFloat(mCurrentValue));
    _SCULK_READ(stream.readFloat(mDefaultMinValue));
    _SCULK_READ(stream.readFloat(mDefaultMaxValue));
    _SCULK_READ(stream.readFloat(mDefaultValue));
    _SCULK_READ(stream.readString(mName));
    return stream.readArray(mModifiers, &Attribute::Modifier::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
