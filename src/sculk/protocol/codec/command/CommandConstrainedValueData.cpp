// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/command/CommandConstrainedValueData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void CommandConstrainedValueData::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(mEnumValueSymbol);
    stream.writeUnsignedInt(mEnumSymbol);
    stream.writeArray(mConstraintIndices, &BinaryStream::writeByte);
}

Result<> CommandConstrainedValueData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt(mEnumValueSymbol));
    _SCULK_READ(stream.readUnsignedInt(mEnumSymbol));
    return stream.readArray(mConstraintIndices, &ReadOnlyBinaryStream::readByte);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
