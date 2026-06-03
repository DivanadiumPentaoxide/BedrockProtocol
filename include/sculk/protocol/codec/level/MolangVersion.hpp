// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/Version.hpp"
#include <cstdint>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

enum class MolangVersion : int {
    Invalid                                = -1,
    BeforeVersioning                       = 0,
    Initial                                = 1,
    FixedItemRemainingUseDurationQuery     = 2,
    ExpressionErrorMessages                = 3,
    UnexpectedOperatorErrors               = 4,
    ConditionalOperatorAssociativity       = 5,
    ComparisonAndLogicalOperatorPrecedence = 6,
    DivideByNegativeValue                  = 7,
    FixedCapeFlapAmountQuery               = 8,
    QueryBlockPropertyRenamedToState       = 9,
    DeprecateOldBlockQueryNames            = 10,
    DeprecatedSnifferAndCamelQueries       = 11,
    LeafSupportingInFirstSolidBlockBelow   = 12,
    CarryingBlockQueryAllActors            = 13,
    NumValidVersions                       = 14,
    Latest                                 = 13,
    HardcodedMolang                        = 13,
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE
