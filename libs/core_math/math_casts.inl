#pragma once
#include "math_casts.h"

#include "core/macros.h"

#include <limits>


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename DST, typename SRC>
DST integer_cast( SRC const src )
{
	static_assert( std::is_integral<DST>::value, "integral_cast only valid on integeral types" );
	DST retVal = integer_truncast<DST>( src );
	RF_ASSERT( retVal == src );
	return retVal;
}



template<typename DST, typename SRC>
DST integer_truncast( SRC const src )
{
	using std::numeric_limits;

	static_assert( std::is_integral<DST>::value, "integral_truncast only valid on integeral types" );

	constexpr DST maxDest = numeric_limits<DST>::max();
	constexpr DST lowestDest = numeric_limits<DST>::lowest();
	constexpr bool couldOverflow = numeric_limits<SRC>::max() > maxDest;
	constexpr bool couldUnderflow = numeric_limits<SRC>::lowest() < lowestDest;

	if( couldOverflow )
	{
		if( src > maxDest )
			return maxDest;
	}

	if( couldUnderflow )
	{
		if( src < lowestDest )
			return lowestDest;
	}

	if( std::is_floating_point<SRC>::value )
	{
		return (DST)roundf( src );
	}
	else
	{
		return (DST)src;
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
