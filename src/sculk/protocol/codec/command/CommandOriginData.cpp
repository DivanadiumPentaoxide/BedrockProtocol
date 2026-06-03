// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/command/CommandOriginData.hpp"
#include "../utility/EnumName.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void CommandOriginData::write(BinaryStream& stream) const {
    utils::writeEnumName(stream, mType);
    mUuid.write(stream);
    stream.writeString(mRequestId);
    stream.writeSignedInt64(mPlayerId);
}

Result<> CommandOriginData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(utils::readEnumName(stream, mType));
    _SCULK_READ(mUuid.read(stream));
    _SCULK_READ(stream.readString(mRequestId));
    return stream.readSignedInt64(mPlayerId);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
