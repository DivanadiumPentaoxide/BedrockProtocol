// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/packet/EducationSettingsPacket.hpp"
#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
#include "../utility/Format.hpp"
#endif

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void EducationSettingsPacket::AgentCapabilities::write(BinaryStream& stream) const {
    stream.writeOptional(mCanModifyBlocks, &BinaryStream::writeBool);
}

Result<> EducationSettingsPacket::AgentCapabilities::read(ReadOnlyBinaryStream& stream) {
    return stream.readOptional(mCanModifyBlocks, &ReadOnlyBinaryStream::readBool);
}

void EducationSettingsPacket::ExternalLinkSettings::write(BinaryStream& stream) const {
    stream.writeString(mUrl);
    stream.writeString(mDisplayName);
}

Result<> EducationSettingsPacket::ExternalLinkSettings::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mUrl));
    return stream.readString(mDisplayName);
}

MinecraftPacketIds EducationSettingsPacket::getId() const noexcept { return MinecraftPacketIds::EducationSettings; }

std::string_view EducationSettingsPacket::getName() const noexcept { return "EducationSettingsPacket"; }

void EducationSettingsPacket::write(BinaryStream& stream) const {
    stream.writeString(mCodeBuilderDefaultUrl);
    stream.writeString(mCodeBuilderTitle);
    stream.writeBool(mCanResizeCodeBuilder);
    stream.writeBool(mDisableLegacyTitleBar);
    stream.writeString(mPostProcessFilter);
    stream.writeString(mScreenshotBorderResourcePath);
    stream.writeOptional(mAgentCapabilities, &AgentCapabilities::write);
    stream.writeOptional(mCodeBuilderOverrideUrl, &BinaryStream::writeString);
    stream.writeBool(mAlwaysFalse);
    stream.writeOptional(mExternalLinkSettings, &ExternalLinkSettings::write);
}

Result<> EducationSettingsPacket::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mCodeBuilderDefaultUrl));
    _SCULK_READ(stream.readString(mCodeBuilderTitle));
    _SCULK_READ(stream.readBool(mCanResizeCodeBuilder));
    _SCULK_READ(stream.readBool(mDisableLegacyTitleBar));
    _SCULK_READ(stream.readString(mPostProcessFilter));
    _SCULK_READ(stream.readString(mScreenshotBorderResourcePath));
    _SCULK_READ(stream.readOptional(mAgentCapabilities, &AgentCapabilities::read));
    _SCULK_READ(stream.readOptional(mCodeBuilderOverrideUrl, &ReadOnlyBinaryStream::readString));
    _SCULK_READ(stream.readBool(mAlwaysFalse));
    return stream.readOptional(mExternalLinkSettings, &ExternalLinkSettings::read);
}

#ifdef SCULK_PROTOCOL_ENABLE_FORMATTING
std::string EducationSettingsPacket::toString() const {
    return SCULK_FORMAT_PACKET(
        SCULK_FORMAT_FIELD(mCodeBuilderDefaultUrl),
        SCULK_FORMAT_FIELD(mCodeBuilderTitle),
        SCULK_FORMAT_FIELD(mCanResizeCodeBuilder),
        SCULK_FORMAT_FIELD(mDisableLegacyTitleBar),
        SCULK_FORMAT_FIELD(mPostProcessFilter),
        SCULK_FORMAT_FIELD(mScreenshotBorderResourcePath),
        SCULK_FORMAT_FIELD(mAgentCapabilities),
        SCULK_FORMAT_FIELD(mCodeBuilderOverrideUrl),
        SCULK_FORMAT_FIELD(mAlwaysFalse),
        SCULK_FORMAT_FIELD(mExternalLinkSettings)
    );
}
#endif

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
