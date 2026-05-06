// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/actor/attribute/FloatAttributeData.hpp"
#include "../../utility/EnumName.hpp"

namespace sculk::protocol::inline abi_v975 {

void FloatAttributeData::write(BinaryStream& stream) const {
    stream.writeFloat(mValue);
    utils::writeEnumName(stream, mOperation);
    stream.writeFloat(mConstraintMin);
    stream.writeFloat(mConstraintMax);
}

Result<> FloatAttributeData::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readFloat(mValue));
    _SCULK_READ(utils::readEnumName(stream, mOperation));
    _SCULK_READ(stream.readFloat(mConstraintMin));
    return stream.readFloat(mConstraintMax);
}

} // namespace sculk::protocol::inline abi_v975
