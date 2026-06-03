// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/level/Experiments.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void Experiments::Experiment::write(BinaryStream& stream) const {
    stream.writeString(mName);
    stream.writeBool(mEnabled);
}

Result<> Experiments::Experiment::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mName));
    return stream.readBool(mEnabled);
}

void Experiments::write(BinaryStream& stream) const {
    stream.writeUnsignedInt(static_cast<std::uint32_t>(mExperiments.size()));
    for (const Experiment& experiment : mExperiments) {
        experiment.write(stream);
    }
    stream.writeBool(mEverToggled);
}

Result<> Experiments::read(ReadOnlyBinaryStream& stream) {
    std::uint32_t experimentCount{};
    _SCULK_READ(stream.readUnsignedInt(experimentCount));
    mExperiments.resize(experimentCount);
    for (Experiment& experiment : mExperiments) {
        _SCULK_READ(experiment.read(stream));
    }
    return stream.readBool(mEverToggled);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
