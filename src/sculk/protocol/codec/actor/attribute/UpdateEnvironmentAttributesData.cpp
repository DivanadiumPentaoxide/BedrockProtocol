// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/UpdateEnvironmentAttributesData.hpp"

namespace sculk::protocol::inline abi_v975 {

void UpdateEnvironmentAttributesData::write(BinaryStream& stream) const {
    stream.writeString(mAttributeLayerName);
    stream.writeVarInt(mAttributeLayerDimension);
    stream.writeArray(mAttributes, &EnvironmentAttributeData::write);
}

Result<> UpdateEnvironmentAttributesData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readString(mAttributeLayerName));
    _SCULK_READ(stream.readVarInt(mAttributeLayerDimension));
    return stream.readArray(mAttributes, &EnvironmentAttributeData::read);
}

} // namespace sculk::protocol::inline abi_v975
