// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/inventory/container/LegacySetItemSlot.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void LegacySetItemSlot::write(BinaryStream& stream) const {
    stream.writeEnum(mContainerEnum, &BinaryStream::writeByte);
    stream.writeArray(mSlots, &BinaryStream::writeByte);
}

Result<> LegacySetItemSlot::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mContainerEnum, &ReadOnlyBinaryStream::readByte));
    return stream.readArray(mSlots, &ReadOnlyBinaryStream::readByte);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
