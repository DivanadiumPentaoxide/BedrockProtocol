// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/GameTestResultsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds GameTestResultsPacket::getId() const noexcept { return MinecraftPacketIds::GameTestResults; }

std::string_view GameTestResultsPacket::getName() const noexcept { return "GameTestResultsPacket"; }

void GameTestResultsPacket::write(BinaryStream& stream) const {
    stream.writeBool(mSucceeded);
    stream.writeString(mError);
    stream.writeString(mTestName);
}

Result<> GameTestResultsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readBool(mSucceeded));
    _SCULK_READ(stream.readString(mError));
    return stream.readString(mTestName);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string GameTestResultsPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mSucceeded),
        SCULK_FORMAT_FIELD(mError),
        SCULK_FORMAT_FIELD(mTestName)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
