// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/actor/attribute/RemoveEnvironmentAttributesData.hpp"
#include "sculk/protocol/codec/actor/attribute/UpdateAttributeLayerSettingsData.hpp"
#include "sculk/protocol/codec/actor/attribute/UpdateAttributeLayersData.hpp"
#include "sculk/protocol/codec/actor/attribute/UpdateEnvironmentAttributesData.hpp"
#include <variant>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

struct AttributeLayerSyncData {
    std::variant<
        UpdateAttributeLayersData,
        UpdateAttributeLayerSettingsData,
        UpdateEnvironmentAttributesData,
        RemoveEnvironmentAttributesData>
        mData{};

    void write(BinaryStream& stream) const;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
