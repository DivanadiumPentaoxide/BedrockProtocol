// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/ResourcePackStackPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ResourcePackStackPacket::PackInfo::write(BinaryStream& stream) const {
    stream.writeString(mId);
    stream.writeString(mVersion);
    stream.writeString(mSubPackName);
}

Result<> ResourcePackStackPacket::PackInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mId));
    _SCULK_READ(stream.readString(mVersion));
    return stream.readString(mSubPackName);
}

MinecraftPacketIds ResourcePackStackPacket::getId() const noexcept { return MinecraftPacketIds::ResourcePackStack; }

std::string_view ResourcePackStackPacket::getName() const noexcept { return "ResourcePackStackPacket"; }

void ResourcePackStackPacket::write(BinaryStream& stream) const {
    stream.writeBool(mTexturePackRequired);
    stream.writeArray(mTexturePackList, &PackInfo::write);
    stream.writeString(mBaseGameVersion);
    mExperiments.write(stream);
    stream.writeBool(mIncludeEditorPacks);
}

Result<> ResourcePackStackPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readBool(mTexturePackRequired));
    _SCULK_READ(stream.readArray(mTexturePackList, &PackInfo::read));
    _SCULK_READ(stream.readString(mBaseGameVersion));
    _SCULK_READ(mExperiments.read(stream));
    return stream.readBool(mIncludeEditorPacks);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string ResourcePackStackPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mTexturePackRequired),
        SCULK_FORMAT_FIELD(mAddonList),
        SCULK_FORMAT_FIELD(mTexturePackList),
        SCULK_FORMAT_FIELD(mBaseGameVersion),
        SCULK_FORMAT_FIELD(mExperiments),
        SCULK_FORMAT_FIELD(mIncludeEditorPacks)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
