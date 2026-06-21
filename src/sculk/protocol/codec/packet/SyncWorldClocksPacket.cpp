// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/SyncWorldClocksPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds SyncWorldClocksPacket::getId() const noexcept { return MinecraftPacketIds::SyncWorldClocks; }

std::string_view SyncWorldClocksPacket::getName() const noexcept { return "SyncWorldClocksPacket"; }

void SyncWorldClocksPacket::write(BinaryStream& stream) const {
    stream.writeVariant(
        mData,
        Overload{
            [&](const SyncStateData& body) { stream.writeArray(body.mClockData, &SyncWorldClockStateData::write); },
            [&](const InitializeRegistryData& body) { stream.writeArray(body.mClockData, &WorldClockData::write); },
            [&](const AddTimeMarkerData& body) {
                stream.writeUnsignedVarInt64(body.mClockId);
                stream.writeArray(body.mTimeMarkers, &TimeMarkerData::write);
            },
            [&](const RemoveTimeMarkerData& body) {
                stream.writeUnsignedVarInt64(body.mClockId);
                stream.writeArray(body.mTimeMarkerIds, &BinaryStream::writeUnsignedVarInt64);
            },
        }
    );
}

Result<> SyncWorldClocksPacket::read(ReadOnlyBinaryStream& stream) {
    return stream.readVariant(
        mData,
        Overload{
            [&](SyncStateData& body) { return stream.readArray(body.mClockData, &SyncWorldClockStateData::read); },
            [&](InitializeRegistryData& body) { return stream.readArray(body.mClockData, &WorldClockData::read); },
            [&](AddTimeMarkerData& body) {
                _SCULK_READ(stream.readUnsignedVarInt64(body.mClockId));
                return stream.readArray(body.mTimeMarkers, &TimeMarkerData::read);
            },
            [&](RemoveTimeMarkerData& body) {
                _SCULK_READ(stream.readUnsignedVarInt64(body.mClockId));
                return stream.readArray(body.mTimeMarkerIds, &ReadOnlyBinaryStream::readUnsignedVarInt64);
            },
        }
    );
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string SyncWorldClocksPacket::toString() const { return SCULK_FORMAT_PACKET(SCULK_FORMAT_FIELD(mData)); }
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
