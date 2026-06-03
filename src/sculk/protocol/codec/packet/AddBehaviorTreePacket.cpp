// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AddBehaviorTreePacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AddBehaviorTreePacket::getId() const noexcept { return MinecraftPacketIds::AddBehaviorTree; }

std::string_view AddBehaviorTreePacket::getName() const noexcept { return "AddBehaviorTreePacket"; }

void AddBehaviorTreePacket::write(BinaryStream& stream) const { stream.writeString(mBehaviorTreeStructure); }

Result<> AddBehaviorTreePacket::read(ReadOnlyBinaryStream& stream) { return stream.readString(mBehaviorTreeStructure); }

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AddBehaviorTreePacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mBehaviorTreeStructure));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
