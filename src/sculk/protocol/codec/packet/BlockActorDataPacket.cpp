// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/BlockActorDataPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds BlockActorDataPacket::getId() const noexcept { return MinecraftPacketIds::BlockActorData; }

std::string_view BlockActorDataPacket::getName() const noexcept { return "BlockActorDataPacket"; }

void BlockActorDataPacket::write(BinaryStream& stream) const {
    mBlockPosition.write(stream);
    mActorDataTags.write(stream);
}

Result<> BlockActorDataPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mBlockPosition.read(stream));
    return mActorDataTags.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string BlockActorDataPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mBlockPosition), SCULK_FORMAT_FIELD(mActorDataTags));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
