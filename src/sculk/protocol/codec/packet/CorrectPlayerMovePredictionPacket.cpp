// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CorrectPlayerMovePredictionPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CorrectPlayerMovePredictionPacket::getId() const noexcept {
    return MinecraftPacketIds::CorrectPlayerMovePrediction;
}

std::string_view CorrectPlayerMovePredictionPacket::getName() const noexcept {
    return "CorrectPlayerMovePredictionPacket";
}

void CorrectPlayerMovePredictionPacket::write(BinaryStream& stream) const {
    stream.writeEnum(mPredictionType, &BinaryStream::writeByte);
    mPos.write(stream);
    mPosDelta.write(stream);
    mVehiclePrediction.write(stream);
    stream.writeOptional(mVehicleAngularVelocity, &BinaryStream::writeFloat);
    stream.writeBool(mOnGround);
    stream.writeUnsignedVarInt64(mPlayerInputTick);
}

Result<> CorrectPlayerMovePredictionPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readEnum(mPredictionType, &ReadOnlyBinaryStream::readByte));
    _SCULK_READ(mPos.read(stream));
    _SCULK_READ(mPosDelta.read(stream));
    _SCULK_READ(mVehiclePrediction.read(stream));
    _SCULK_READ(stream.readOptional(mVehicleAngularVelocity, &ReadOnlyBinaryStream::readFloat));
    _SCULK_READ(stream.readBool(mOnGround));
    return stream.readUnsignedVarInt64(mPlayerInputTick);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CorrectPlayerMovePredictionPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPredictionType),
        SCULK_FORMAT_FIELD(mPos),
        SCULK_FORMAT_FIELD(mPosDelta),
        SCULK_FORMAT_FIELD(mVehiclePrediction),
        SCULK_FORMAT_FIELD(mVehicleAngularVelocity),
        SCULK_FORMAT_FIELD(mOnGround),
        SCULK_FORMAT_FIELD(mPlayerInputTick)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
