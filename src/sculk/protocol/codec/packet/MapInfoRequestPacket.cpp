// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/MapInfoRequestPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void MapInfoRequestPacket::ClientPixel::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(mPixel);
    stream.writeUnsignedShort(mIndex);
}

Result<> MapInfoRequestPacket::ClientPixel::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readUnsignedInt(mPixel));
    return stream.readUnsignedShort(mIndex);
}

MinecraftPacketIds MapInfoRequestPacket::getId() const noexcept { return MinecraftPacketIds::MapInfoRequest; }

std::string_view MapInfoRequestPacket::getName() const noexcept { return "MapInfoRequestPacket"; }

void MapInfoRequestPacket::write(BinaryStream& stream) const {
    stream.writeVarInt64(mMapUniqueId);
    stream.writeUnsignedInt(static_cast<std::uint32_t>(mClientPixels.size()));
    for (const auto& pixel : mClientPixels) {
        pixel.write(stream);
    }
}

Result<> MapInfoRequestPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVarInt64(mMapUniqueId));
    std::uint32_t pixelCount{};
    _SCULK_READ(stream.readUnsignedInt(pixelCount));
    mClientPixels.clear();
    mClientPixels.resize(pixelCount);
    for (auto& pixel : mClientPixels) {
        _SCULK_READ(pixel.read(stream));
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string MapInfoRequestPacket::toString() const {
    return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mMapUniqueId), SCULK_FORMAT_FIELD(mClientPixels));
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
