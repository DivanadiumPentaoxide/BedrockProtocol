// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ContainerRegistryCleanupPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ContainerRegistryCleanupPacket::getId() const noexcept {
    return MinecraftPacketIds::ContainerRegistryCleanup;
}

std::string_view ContainerRegistryCleanupPacket::getName() const noexcept { return "ContainerRegistryCleanupPacket"; }

void ContainerRegistryCleanupPacket::write(BinaryStream& stream) const {
    stream.writeArray(mRemovedContainers, &FullContainerName::write);
}

Result<> ContainerRegistryCleanupPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readArray(mRemovedContainers, &FullContainerName::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ContainerRegistryCleanupPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mRemovedContainers));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
