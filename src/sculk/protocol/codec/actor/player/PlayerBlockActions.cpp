// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/player/PlayerBlockActions.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void PlayerBlockActionData::write(BinaryStream& stream) const {
    stream.writeEnum(mActionType, &BinaryStream::writeVarInt);
    mPosition.write(stream);
    stream.writeVarInt(mFacing);
}

Result<> PlayerBlockActionData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mActionType, &ReadOnlyBinaryStream::readVarInt));
    _SCULK_READ(mPosition.read(stream));
    return stream.readVarInt(mFacing);
}

void PlayerBlockActions::write(BinaryStream& stream) const {
    stream.writeVarInt(static_cast<int>(mActions.size()));
    for (const auto& action : mActions) {
        action.write(stream);
    }
}

Result<> PlayerBlockActions::read(ReadOnlyBinaryStream& stream) {
    int actionCount{};
    _SCULK_READ(stream.readVarInt(actionCount));
    mActions.resize(static_cast<std::size_t>(actionCount));
    for (auto& action : mActions) {
        _SCULK_READ(action.read(stream));
    }
    return {};
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
