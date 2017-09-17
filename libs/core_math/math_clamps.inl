#pragma once
#include "math_clamps.h"

#include "core/macros.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
constexpr TYPE const& Min(TYPE const& lhs, TYPE const& rhs)
{
	return lhs < rhs ? lhs : rhs;
}



template<typename TYPE>
constexpr TYPE const& Max( TYPE const& lhs, TYPE const& rhs )
{
	return lhs > rhs ? lhs : rhs;
}



template<typename TYPE>
constexpr TYPE const& Clamp( TYPE const& min, TYPE const& value, TYPE const& max )
{
	RF_ASSERT( min <= max );
	return Min( Max( min, value ), max );
}



template<typename TYPE>
constexpr TYPE SnapNearest( TYPE const& value, TYPE const& step )
{
	// NOTE: Need to create a constexpr-capable modf for floats/doubles
	static_assert( std::is_integral<TYPE>::value, "TODO: Floating-point support" );

	RF_ASSERT( step > 0 );
	TYPE const mod = value % step;
	if( mod == 0 )
		return value;

	int64_t const baseMultiplier = static_cast<int64_t>( value / step );
	TYPE const halfStep = step / 2;

	static_assert( ( -3 % 2 ) == -1, "" );
	bool const snapHigh = mod > halfStep;
	if( snapHigh )
	{
		return static_cast<TYPE>( baseMultiplier + 1 ) * step;
	}
	else
	{
		return static_cast<TYPE>( baseMultiplier ) * step;
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
