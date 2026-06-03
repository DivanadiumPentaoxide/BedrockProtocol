// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/ServerConfiguration.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void GatheringsConfigurationJoinInfo::write(BinaryStream& stream) const {
    mExperienceId.write(stream);
    stream.writeString(mExperienceName);
    mExperienceWorldId.write(stream);
    stream.writeString(mExperienceWorldName);
    stream.writeString(mCreatorId);
    mTargetId.write(stream);
    stream.writeString(mScenarioId);
    stream.writeString(mServerId);
}

Result<> GatheringsConfigurationJoinInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(mExperienceId.read(stream));
    _SCULK_READ(stream.readString(mExperienceName));
    _SCULK_READ(mExperienceWorldId.read(stream));
    _SCULK_READ(stream.readString(mExperienceWorldName));
    _SCULK_READ(stream.readString(mCreatorId));
    _SCULK_READ(mTargetId.read(stream));
    _SCULK_READ(stream.readString(mScenarioId));
    return stream.readString(mServerId);
}

void GatheringsConfigurationClientStoreEntryPointInfo::write(BinaryStream& stream) const {
    stream.writeString(mStoreId);
    stream.writeString(mStoreName);
}

Result<> GatheringsConfigurationClientStoreEntryPointInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mStoreId));
    return stream.readString(mStoreName);
}

void GatheringsConfigurationPresenceInfo::write(BinaryStream& stream) const {
    stream.writeString(mExperienceName);
    stream.writeString(mWorldName);
}

Result<> GatheringsConfigurationPresenceInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mExperienceName));
    return stream.readString(mWorldName);
}

void ServerConfigurationJoinInfo::write(BinaryStream& stream) const {
    stream.writeOptional(mGatheringsConfiguration, &GatheringsConfigurationJoinInfo::write);
    stream.writeOptional(mStoreEntryPointInfo, &GatheringsConfigurationClientStoreEntryPointInfo::write);
    stream.writeOptional(mPresenceInfo, &GatheringsConfigurationPresenceInfo::write);
}

Result<> ServerConfigurationJoinInfo::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readOptional(mGatheringsConfiguration, &GatheringsConfigurationJoinInfo::read));
    _SCULK_READ(stream.readOptional(mStoreEntryPointInfo, &GatheringsConfigurationClientStoreEntryPointInfo::read));
    return stream.readOptional(mPresenceInfo, &GatheringsConfigurationPresenceInfo::read);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
