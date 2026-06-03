// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/SyncedAttribute.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void SyncedAttribute::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeFloat(mMinValue);
    stream.writeFloat(mMaxValue);
    stream.writeFloat(mCurrentValue);
}

Result<> SyncedAttribute::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readFloat(mMinValue));
    _SCULK_READ(stream.readFloat(mMaxValue));
    return stream.readFloat(mCurrentValue);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
