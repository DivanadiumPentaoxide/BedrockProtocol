// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#include "sculk/protocol/codec/math/Vec3.hpp"

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

void Vec3::write(BinaryStream& stream) const {
    stream.writeFloat(mX);
    stream.writeFloat(mY);
    stream.writeFloat(mZ);
}

Result<> Vec3::read(ReadOnlyBinaryStream& stream) {
    _SCULK_READ(stream.readFloat(mX));
    _SCULK_READ(stream.readFloat(mY));
    return stream.readFloat(mZ);
}

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
