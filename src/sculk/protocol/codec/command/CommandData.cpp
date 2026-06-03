// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/command/CommandData.hpp"
#include "../utility/EnumName.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void CommandData::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeString(mDescription);
    stream.writeUnsignedShort(mFlags);
    utils::writeEnumName(stream, mCommandPermissionLevel);
    stream.writeSignedInt(mAliasEnum);
    stream.writeArray(mChainedSubcommandIndexes, &BinaryStream::writeUnsignedInt);
    stream.writeArray(mOverloads, &CommandOverloadData::write);
}

Result<> CommandData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    _SCULK_READ(stream.readString(mDescription));
    _SCULK_READ(stream.readUnsignedShort(mFlags));
    _SCULK_READ(utils::readEnumName(stream, mCommandPermissionLevel));
    _SCULK_READ(stream.readSignedInt(mAliasEnum));
    _SCULK_READ(stream.readArray(mChainedSubcommandIndexes, &ReadOnlyBinaryStream::readUnsignedInt));
    return stream.readArray(mOverloads, &CommandOverloadData::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
