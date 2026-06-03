// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/command/CommandEnumData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void CommandEnumData::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeArray(mValues, &BinaryStream::writeUnsignedInt);
}

Result<> CommandEnumData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    return stream.readArray(mValues, &ReadOnlyBinaryStream::readUnsignedInt);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
