// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CameraAimAssistPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CameraAimAssistPacket::getId() const noexcept { return MinecraftPacketIds::CameraAimAssist; }

std::string_view CameraAimAssistPacket::getName() const noexcept { return "CameraAimAssistPacket"; }

void CameraAimAssistPacket::write(BinaryStream& stream) const {
    stream.writeString(mPresetId);
    mViewAngle.write(stream);
    stream.writeFloat(mDistance);
    stream.writeEnum(mTargetMode, &BinaryStream::writeByte);
    stream.writeEnum(mAction, &BinaryStream::writeByte);
    stream.writeBool(mShowDebugRender);
}

Result<> CameraAimAssistPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mPresetId));
    _SCULK_READ(mViewAngle.read(stream));
    _SCULK_READ(stream.readFloat(mDistance));
    _SCULK_READ(stream.readEnum(mTargetMode, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(stream.readEnum(mAction, &ReadOnlyBinaryStream::readByte));
    return stream.readBool(mShowDebugRender);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CameraAimAssistPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPresetId),
        SCULK_FORMAT_FIELD(mViewAngle),
        SCULK_FORMAT_FIELD(mDistance),
        SCULK_FORMAT_FIELD(mTargetMode),
        SCULK_FORMAT_FIELD(mAction)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
