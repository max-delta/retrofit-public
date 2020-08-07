#pragma once
#include "math_clamps.h"

#include "core/macros.h"

#include "rftl/type_traits"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename TYPE>
constexpr TYPE const& Min( TYPE const& lhs, TYPE const& rhs )
{
	return lhs < rhs ? lhs : rhs;
}



template<typename TYPE>
constexpr TYPE const& Min( TYPE const& lhs, TYPE const& rhs, TYPE const& additional... )
{
	return Min( lhs, Min( rhs, additional ) );
}



template<typename TYPE>
constexpr TYPE const& Max( TYPE const& lhs, TYPE const& rhs )
{
	return lhs > rhs ? lhs : rhs;
}



template<typename TYPE>
constexpr TYPE const& Max( TYPE const& lhs, TYPE const& rhs, TYPE const& additional... )
{
	return Max( lhs, Max( rhs, additional ) );
}



template<typename TYPE>
constexpr TYPE const& Clamp( TYPE const& min, TYPE const& value, TYPE const& max )
{
	if( min > max )
	{
		// Error case
		return value;
	}

	return Min( Max( min, value ), max );
}



template<typename TYPE>
constexpr TYPE SnapNearest( TYPE const& value, TYPE const& step )
{
	// NOTE: Need to create a constexpr-capable modf for floats/doubles
	static_assert( rftl::is_integral<TYPE>::value, "TODO: Floating-point support" );

	if( step <= 0 )
	{
		// Error case
		// As step approaches zero, return value approaches value
		return value;
	}

	TYPE const mod = static_cast<TYPE>( value % step );
	static_assert( ( -3 % 4 ) == -3, "" );
	if( mod == 0 )
		return value;

	int64_t const baseMultiplier = static_cast<int64_t>( value / step );
	static_assert( ( -3 / 4 ) == 0, "" );
	TYPE const halfStep = static_cast<TYPE>( step / 2 );
	static_assert( ( 4 / 2 ) == 2, "" );

	TYPE const modPositive = static_cast<TYPE>( mod >= 0 ? mod : -mod );
	static_assert( -( -3 % 4 ) == 3, "" );
	bool const snapHigh = modPositive > halfStep;
	if( snapHigh )
	{
		if( value >= 0 )
		{
			return static_cast<TYPE>( static_cast<TYPE>( baseMultiplier + 1 ) * step );
		}
		else
		{
			return static_cast<TYPE>( static_cast<TYPE>( baseMultiplier - 1 ) * step );
		}
	}
	else
	{
		return static_cast<TYPE>( static_cast<TYPE>( baseMultiplier ) * step );
	}
}



template<typename TYPE>
constexpr TYPE SnapHighest( TYPE const& value, TYPE const& step )
{
	// NOTE: Need to create a constexpr-capable modf for floats/doubles
	static_assert( rftl::is_integral<TYPE>::value, "TODO: Floating-point support" );

	if( step <= 0 )
	{
		// Error case
		// As step approaches zero, return value approaches value
		return value;
	}

	TYPE const mod = static_cast<TYPE>( value % step );
	if( mod == 0 )
		return value;

	int64_t const baseMultiplier = static_cast<int64_t>( value / step );
	if( mod < 0 )
		return static_cast<TYPE>( value - mod );
	return static_cast<TYPE>( static_cast<TYPE>( baseMultiplier + 1 ) * step );
}



template<typename TYPE>
constexpr TYPE SnapLowest( TYPE const& value, TYPE const& step )
{
	// NOTE: Need to create a constexpr-capable modf for floats/doubles
	static_assert( rftl::is_integral<TYPE>::value, "TODO: Floating-point support" );

	if( step <= 0 )
	{
		// Error case
		// As step approaches zero, return value approaches value
		return value;
	}

	TYPE const mod = static_cast<TYPE>( value % step );
	if( mod == 0 )
		return value;

	int64_t const baseMultiplier = static_cast<int64_t>( value / step );
	if( mod < 0 )
		return static_cast<TYPE>( static_cast<TYPE>( baseMultiplier - 1 ) * step );
	return static_cast<TYPE>( value - mod );
}

///////////////////////////////////////////////////////////////////////////////
}}
