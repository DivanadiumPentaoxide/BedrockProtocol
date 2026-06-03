// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/CommandOutputPacket.hpp"
#include "../utility/EnumName.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

MinecraftPacketIds CommandOutputPacket::getId() const noexcept { return MinecraftPacketIds::CommandOutput; }

std::string_view CommandOutputPacket::getName() const noexcept { return "CommandOutputPacket"; }

void CommandOutputPacket::write(BinaryStream& stream) const {
    mOriginData.write(stream);
    utils::writeEnumName(stream, mOutputType);
    stream.writeUnsignedInt(mSuccessCount);
    stream.writeArray(mOutputMessages, [](BinaryStream& output, const OutputMessage& data) {
        output.writeString(data.mMessageId);
        output.writeBool(data.mSuccess);
        output.writeArray(data.mParameters, &BinaryStream::writeString);
    });
    stream.writeString(mDataSet);
}

Result<> CommandOutputPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mOriginData.read(stream));
    _SCULK_READ(utils::readEnumName(stream, mOutputType));
    _SCULK_READ(stream.readUnsignedInt(mSuccessCount));
    _SCULK_READ(stream.readArray(mOutputMessages, [](ReadOnlyBinaryStream& input, OutputMessage& data) {
        _SCULK_READ(input.readString(data.mMessageId));
        _SCULK_READ(input.readBool(data.mSuccess));
        return input.readArray(data.mParameters, &ReadOnlyBinaryStream::readString);
    }));
    return stream.readString(mDataSet);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string CommandOutputPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mOriginData),
        SCULK_FORMAT_FIELD(mOutputType),
        SCULK_FORMAT_FIELD(mSuccessCount),
        SCULK_FORMAT_FIELD(mOutputMessages),
        SCULK_FORMAT_FIELD(mDataSet)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
