// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/AttributeData.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void AttributeData::write(BinaryStream& stream) const {
    stream.writeVariant(mData, [&stream](const auto& data) { data.write(stream); });
}

Result<> AttributeData::read(ReadOnlyBinaryStream& stream) {
    return stream.readVariant(mData, [&stream](auto& data) { return data.read(stream); });
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
