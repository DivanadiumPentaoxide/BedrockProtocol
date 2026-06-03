// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/PlayerAuthInputPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds PlayerAuthInputPacket::getId() const noexcept { return MinecraftPacketIds::PlayerAuthInput; }

std::string_view PlayerAuthInputPacket::getName() const noexcept { return "PlayerAuthInputPacket"; }

void PlayerAuthInputPacket::write(BinaryStream& stream) const {
    mPlayerRotation.write(stream);
    mPosition.write(stream);
    mMoveVector.write(stream);
    stream.writeFloat(mPlayerHeadRotation);
    stream.writeBitset(mInputData);
    stream.writeUnsignedVarInt(mInputType);
    stream.writeUnsignedVarInt(mPlayMode);
    stream.writeUnsignedVarInt(mNewInteractionModel);
    mInteractRotation.write(stream);
    stream.writeUnsignedVarInt64(mClientTick);
    mPosDelta.write(stream);
    if (mInputData.test(static_cast<std::size_t>(PlayerAuthInputData::PerformItemInteraction))) {
        mItemUseTransaction.write(stream);
    }
    if (mInputData.test(static_cast<std::size_t>(PlayerAuthInputData::PerformItemStackRequest))) {
        mItemStackRequestData.write(stream);
    }
    if (mInputData.test(static_cast<std::size_t>(PlayerAuthInputData::PerformBlockActions))) {
        mPlayerBlockActions.write(stream);
    }
    if (mInputData.test(static_cast<std::size_t>(PlayerAuthInputData::IsInClientPredictedVehicle))) {
        mVehicleRotation.write(stream);
        stream.writeVarInt64(mClientPredictedVihicle);
    }
    mAnologMoveVector.write(stream);
    mCameraOrientation.write(stream);
    mRawMoveVector.write(stream);
}

Result<> PlayerAuthInputPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mPlayerRotation.read(stream));
    _SCULK_READ(mPosition.read(stream));
    _SCULK_READ(mMoveVector.read(stream));
    _SCULK_READ(stream.readFloat(mPlayerHeadRotation));
    _SCULK_READ(stream.readBitset(mInputData));
    _SCULK_READ(stream.readUnsignedVarInt(mInputType));
    _SCULK_READ(stream.readUnsignedVarInt(mPlayMode));
    _SCULK_READ(stream.readUnsignedVarInt(mNewInteractionModel));
    _SCULK_READ(mInteractRotation.read(stream));
    _SCULK_READ(stream.readUnsignedVarInt64(mClientTick));
    _SCULK_READ(mPosDelta.read(stream));
    if (mInputData.test(static_cast<std::size_t>(PlayerAuthInputData::PerformItemInteraction))) {
        _SCULK_READ(mItemUseTransaction.read(stream));
    }
    if (mInputData.test(static_cast<std::size_t>(PlayerAuthInputData::PerformItemStackRequest))) {
        _SCULK_READ(mItemStackRequestData.read(stream));
    }
    if (mInputData.test(static_cast<std::size_t>(PlayerAuthInputData::PerformBlockActions))) {
        _SCULK_READ(mPlayerBlockActions.read(stream));
    }
    if (mInputData.test(static_cast<std::size_t>(PlayerAuthInputData::IsInClientPredictedVehicle))) {
        _SCULK_READ(mVehicleRotation.read(stream));
        _SCULK_READ(stream.readVarInt64(mClientPredictedVihicle));
    }
    _SCULK_READ(mAnologMoveVector.read(stream));
    _SCULK_READ(mCameraOrientation.read(stream));
    return mRawMoveVector.read(stream);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string PlayerAuthInputPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mPlayerRotation),
        SCULK_FORMAT_FIELD(mPosition),
        SCULK_FORMAT_FIELD(mMoveVector),
        SCULK_FORMAT_FIELD(mPlayerHeadRotation),
        SCULK_FORMAT_FIELD(mInputData),
        SCULK_FORMAT_FIELD(mInputType),
        SCULK_FORMAT_FIELD(mPlayMode),
        SCULK_FORMAT_FIELD(mNewInteractionModel),
        SCULK_FORMAT_FIELD(mInteractRotation),
        SCULK_FORMAT_FIELD(mClientTick),
        SCULK_FORMAT_FIELD(mPosDelta),
        SCULK_FORMAT_FIELD(mItemUseTransaction),
        SCULK_FORMAT_FIELD(mItemStackRequestData),
        SCULK_FORMAT_FIELD(mPlayerBlockActions),
        SCULK_FORMAT_FIELD(mVehicleRotation),
        SCULK_FORMAT_FIELD(mClientPredictedVihicle),
        SCULK_FORMAT_FIELD(mAnologMoveVector),
        SCULK_FORMAT_FIELD(mCameraOrientation),
        SCULK_FORMAT_FIELD(mRawMoveVector)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
