// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/AttributeLayerSyncData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void AttributeLayerSyncData::write(BinaryStream& stream) const {
    stream.writeVariantIndex<std::uint32_t>(mData, &BinaryStream::writeUnsignedVarInt);
    std::visit([&](const auto& value) { value.write(stream); }, mData);
}

Result<> AttributeLayerSyncData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readVariantIndex<std::uint32_t>(mData, &ReadOnlyBinaryStream::readUnsignedVarInt));
    return std::visit([&](auto& value) { return value.read(stream); }, mData);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
