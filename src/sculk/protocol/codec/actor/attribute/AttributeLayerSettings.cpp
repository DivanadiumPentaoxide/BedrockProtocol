// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/AttributeLayerSettings.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void AttributeLayerWeight::write(BinaryStream& stream) const {
    stream.writeVariant(
        mWeight,
        Overload{
            [&](float value) { stream.writeFloat(value); },
            [&](const std::string& value) { stream.writeString(value); },
        }
    );
}

Result<> AttributeLayerWeight::read(ReadOnlyBinaryStream& stream) {
    return stream.readVariant(
        mWeight,
        Overload{
            [&](float& value) { return stream.readFloat(value); },
            [&](std::string& value) { return stream.readString(value); },
        }
    );
}

void AttributeLayerSettings::write(BinaryStream& stream) const {
    stream.writeSignedInt(mPriority);
    mWeight.write(stream);
    stream.writeBool(mEnabled);
    stream.writeBool(mTransitionsPaused);
}

Result<> AttributeLayerSettings::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readSignedInt(mPriority));
    _SCULK_READ(mWeight.read(stream));
    _SCULK_READ(stream.readBool(mEnabled));
    return stream.readBool(mTransitionsPaused);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
