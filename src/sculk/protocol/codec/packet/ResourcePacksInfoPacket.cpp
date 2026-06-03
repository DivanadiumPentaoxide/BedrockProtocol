// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ResourcePacksInfoPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds ResourcePacksInfoPacket::getId() const noexcept { return MinecraftPacketIds::ResourcePacksInfo; }

std::string_view ResourcePacksInfoPacket::getName() const noexcept { return "ResourcePacksInfoPacket"; }

void ResourcePacksInfoPacket::write(BinaryStream& stream) const {
    stream.writeBool(mResourcePackRequired);
    stream.writeBool(mHasAddonPacks);
    stream.writeBool(mHasScripts);
    stream.writeBool(mIsVibrantVisualsForceDisabled);
    mWorldTemplateId.write(stream);
    stream.writeString(mWorldTemplateVersion);
    stream.writeUnsignedShort(static_cast<std::uint16_t>(mResourcePacks.size()));
    for (const auto& pack : mResourcePacks) {
        pack.write(stream);
    }
}

Result<> ResourcePacksInfoPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readBool(mResourcePackRequired));
    _SCULK_READ(stream.readBool(mHasAddonPacks));
    _SCULK_READ(stream.readBool(mHasScripts));
    _SCULK_READ(stream.readBool(mIsVibrantVisualsForceDisabled));
    _SCULK_READ(mWorldTemplateId.read(stream));
    _SCULK_READ(stream.readString(mWorldTemplateVersion));
    std::uint16_t size{};
    _SCULK_READ(stream.readUnsignedShort(size));
    mResourcePacks.resize(size);
    for (auto& pack : mResourcePacks) {
        _SCULK_READ(pack.read(stream));
    }
    return {};
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ResourcePacksInfoPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mResourcePackRequired),
        SCULK_FORMAT_FIELD(mHasAddonPacks),
        SCULK_FORMAT_FIELD(mHasScripts),
        SCULK_FORMAT_FIELD(mIsVibrantVisualsForceDisabled),
        SCULK_FORMAT_FIELD(mWorldTemplateId),
        SCULK_FORMAT_FIELD(mWorldTemplateVersion),
        SCULK_FORMAT_FIELD(mResourcePacks)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
