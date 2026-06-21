// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/AvailableCommandsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds AvailableCommandsPacket::getId() const noexcept { return MinecraftPacketIds::AvailableCommands; }

std::string_view AvailableCommandsPacket::getName() const noexcept { return "AvailableCommandsPacket"; }

void AvailableCommandsPacket::write(BinaryStream& stream) const {
    stream.writeArray(mEnumValues, &BinaryStream::writeString);
    stream.writeArray(mChainedSubcommandsValues, &BinaryStream::writeString);
    stream.writeArray(mPostFixes, &BinaryStream::writeString);
    stream.writeArray(mEnumData, &CommandEnumData::write);
    stream.writeArray(mChainedSubcommandData, &CommandChainedSubcommandData::write);
    stream.writeArray(mCommands, &CommandData::write);
    stream.writeArray(mSoftEnums, &CommandSoftEnumData::write);
    stream.writeArray(mConstraints, &CommandConstrainedValueData::write);
}

Result<> AvailableCommandsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readArray(mEnumValues, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readArray(mChainedSubcommandsValues, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readArray(mPostFixes, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readArray(mEnumData, &CommandEnumData::read));
    _SCULK_READ(stream.readArray(mChainedSubcommandData, &CommandChainedSubcommandData::read));
    _SCULK_CHECK_MAX(mChainedSubcommandData.size(), 16)
    _SCULK_READ(stream.readArray(mCommands, &CommandData::read));
    _SCULK_READ(stream.readArray(mSoftEnums, &CommandSoftEnumData::read));
    return stream.readArray(mConstraints, &CommandConstrainedValueData::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string AvailableCommandsPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mEnumValues),
        SCULK_FORMAT_FIELD(mChainedSubcommandsValues),
        SCULK_FORMAT_FIELD(mPostFixes),
        SCULK_FORMAT_FIELD(mEnumData),
        SCULK_FORMAT_FIELD(mChainedSubcommandData),
        SCULK_FORMAT_FIELD(mCommands),
        SCULK_FORMAT_FIELD(mSoftEnums),
        SCULK_FORMAT_FIELD(mConstraints)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
