// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/EnvironmentAttributeData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void EnvironmentAttributeData::write(BinaryStream& stream) const {
    stream.writeString(mAttributeName);
    stream.writeOptional(mFromAttribute, &AttributeData::write);
    mAttribute.write(stream);
    stream.writeOptional(mToAttribute, &AttributeData::write);
    stream.writeUnsignedInt(mCurrentTransitionTicks);
    stream.writeUnsignedInt(mTotalTransitionTicks);
    stream.writeEnum(mEasing, &BinaryStream::writeSignedInt);
}

Result<> EnvironmentAttributeData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mAttributeName));
    _SCULK_READ(stream.readOptional(mFromAttribute, &AttributeData::read));
    _SCULK_READ(mAttribute.read(stream));
    _SCULK_READ(stream.readOptional(mToAttribute, &AttributeData::read));
    _SCULK_READ(stream.readUnsignedInt(mCurrentTransitionTicks));
    _SCULK_READ(stream.readUnsignedInt(mTotalTransitionTicks));
    return stream.readEnum(mEasing, &ReadOnlyBinaryStream::readSignedInt);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
