/*
 * Copyright (C) 2024 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "CSSParserContext.h"
#include "CSSProperty.h"
#include "CSSPropertyParserConsumer+MetaConsumerDefinitions.h"
#include "CSSPropertyParserState.h"

namespace WebCore {
namespace CSSPropertyParserHelpers {

struct LengthValidator {
    static constexpr std::optional<CSS::LengthUnit> validate(CSSUnitType unitType, CSS::PropertyParserState& state, CSSPropertyParserOptions options)
    {
        if (unitType == CSSUnitType::CSS_QUIRKY_EM && !isUASheetBehavior(options.overrideParserMode.value_or(state.context.mode)))
            return std::nullopt;
        return CSS::UnitTraits<CSS::LengthUnit>::validate(unitType);
    }

    template<auto R, typename V> static bool isValid(CSS::LengthRaw<R, V> raw, CSSPropertyParserOptions)
    {
        // Values other than 0 and +/-∞ are not supported for <length> numeric ranges currently.
        return isValidNonCanonicalizableDimensionValue(raw);
    }

    static bool shouldAcceptUnitlessValue(double value, CSS::PropertyParserState& state, CSSPropertyParserOptions options)
    {
        if (!value && options.unitlessZeroLength == UnitlessZeroQuirk::Allow)
            return true;

        auto mode = options.overrideParserMode.value_or(state.context.mode);

        if (isUnitlessValueParsingForcedForMode(mode))
            return true;

        return mode == HTMLQuirksMode && CSSProperty::acceptsQuirkyLength(state.currentProperty);
    }
};

template<auto R, typename V> struct ConsumerDefinition<CSS::Length<R, V>> {
    using FunctionToken = FunctionConsumerForCalcValues<CSS::Length<R, V>>;
    using DimensionToken = DimensionConsumer<CSS::Length<R, V>, LengthValidator>;
    using NumberToken = NumberConsumerForUnitlessValues<CSS::Length<R, V>, LengthValidator, CSS::LengthUnit::Px>;
};

} // namespace CSSPropertyParserHelpers
} // namespace WebCore
