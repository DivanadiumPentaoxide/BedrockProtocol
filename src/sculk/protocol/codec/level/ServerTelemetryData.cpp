// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/ServerTelemetryData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void ServerTelemetryData::write(BinaryStream& stream) const {
    stream.writeString(mServerId);
    stream.writeString(mScenarioId);
    stream.writeString(mWorldId);
    stream.writeString(mOwnerId);
}

Result<> ServerTelemetryData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mServerId));
    _SCULK_READ(stream.readString(mScenarioId));
    _SCULK_READ(stream.readString(mWorldId));
    return stream.readString(mOwnerId);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
