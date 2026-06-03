// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/resource/PackInfoData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void PackInfoData::write(BinaryStream& stream) const {
    mPackId.write(stream);
    stream.writeString(mPackVersion);
    stream.writeUnsignedInt64(mPackSize);
    stream.writeString(mContentKey);
    stream.writeString(mSubpackName);
    stream.writeString(mContentIdentity);
    stream.writeBool(mHasScripts);
    stream.writeBool(mIsAddonPack);
    stream.writeBool(mIsRayTracingCapable);
    stream.writeString(mCDNUrl);
}

Result<> PackInfoData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPackId.read(stream));
    _SCULK_READ(stream.readString(mPackVersion));
    _SCULK_READ(stream.readUnsignedInt64(mPackSize));
    _SCULK_READ(stream.readString(mContentKey));
    _SCULK_READ(stream.readString(mSubpackName));
    _SCULK_READ(stream.readString(mContentIdentity));
    _SCULK_READ(stream.readBool(mHasScripts));
    _SCULK_READ(stream.readBool(mIsAddonPack));
    _SCULK_READ(stream.readBool(mIsRayTracingCapable));
    return stream.readString(mCDNUrl);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
