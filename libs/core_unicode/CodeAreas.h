#pragma once

#include "core_unicode/UnicodeFwd.h"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Will return values in the range 0-31, but only 0-16 are valid
constexpr Plane::Value DeterminePlane( char32_t ch );

// NOTE: Will return 0 for non-private code points
constexpr PrivateUseArea::Value DeterminePrivateUseArea( char32_t ch );

///////////////////////////////////////////////////////////////////////////////
}

#include "CodeAreas.inl"
