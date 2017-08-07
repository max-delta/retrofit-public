#pragma once
#include <stdint.h>
#include <type_traits>

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename DST, typename SRC>
DST integer_cast( SRC const src );

template<typename DST, typename SRC>
DST integer_truncast( SRC const src );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_casts.inl"
