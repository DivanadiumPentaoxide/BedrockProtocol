// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/GuiDataPickItemPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds GuiDataPickItemPacket::getId() const noexcept { return MinecraftPacketIds::GuiDataPickItem; }

std::string_view GuiDataPickItemPacket::getName() const noexcept { return "GuiDataPickItemPacket"; }

void GuiDataPickItemPacket::write(BinaryStream& stream) const {
    stream.writeString(mItemName);
    stream.writeString(mItemEffectName);
    stream.writeSignedInt(mSlot);
}

Result<> GuiDataPickItemPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mItemName));
    _SCULK_READ(stream.readString(mItemEffectName));
    return stream.readSignedInt(mSlot);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string GuiDataPickItemPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mItemName),
        SCULK_FORMAT_FIELD(mItemEffectName),
        SCULK_FORMAT_FIELD(mSlot)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
