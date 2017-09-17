#pragma once
#include <stdint.h>
#include <type_traits>

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename DST, typename SRC>
DST integer_cast( SRC const src );

template<typename DST, typename SRC>
DST integer_truncast( SRC const src );

template<typename DST, typename SRC,
	typename std::enable_if<std::is_integral<DST>::value,int>::type = 0>
DST real_cast( SRC const src );

template<typename DST, typename SRC,
	typename std::enable_if<std::is_floating_point<DST>::value,int>::type = 0>
DST real_cast( SRC const src );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_casts.inl"
