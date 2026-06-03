// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/BoolAttributeData.hpp"
#include "../../utility/EnumName.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void BoolAttributeData::write(BinaryStream& stream) const {
    stream.writeBool(mValue);
    utils::writeEnumName(stream, mOperation);
}

Result<> BoolAttributeData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readBool(mValue));
    return utils::readEnumName(stream, mOperation);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
