#pragma once
#include "math_clamps.h"

#include "core/macros.h"

#include "rftl/type_traits"
#include "rftl/limits"


namespace RF::math {
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



template<typename BASE, typename OFFSET>
constexpr BASE WrapPositiveOffset( BASE const& value, BASE const& mod, OFFSET const& offset )
{
	// NOTE: Need to create a constexpr-capable modf for floats/doubles
	static_assert( rftl::is_integral<BASE>::value, "TODO: Floating-point version" );

	// Need this to allow safe casting to base type
	static_assert( rftl::numeric_limits<OFFSET>::max() <= rftl::numeric_limits<BASE>::max(),
		"Offset can't exceed base. Consider integer_cast beforehand." );

	// Unsigned integer overflow is defined to be equivalent to a mod of one
	//  past the max value of the type
	// NOTE: Signed integer overflow is undefined at time of writing
	static_assert( rftl::is_unsigned<BASE>::value,
		"Base must be unsigned to prevent undefined behavior. Consider integer_cast beforehand." );

	// NOTE: Casts on operations with same types are safe, and pointless, but
	//  required to workaround archaic integer promotion rules from mistakes
	//  made in the dark ages of primitive C.

	if( offset >= 0 )
	{
		// Cast safe, see above assertion on max size
		BASE const positiveOffset = static_cast<BASE>( offset );

		// Safe, only reduces size
		BASE const minimalOffset = static_cast<BASE>( positiveOffset % mod );

		// Cast safe, see above assertion on unsigned rollover behavior
		BASE const rawOffset = static_cast<BASE>( value + minimalOffset );
		bool const overflow = rawOffset < value;
		if( overflow )
		{
			// Safe, mod can't exceed max
			BASE const spillover = static_cast<BASE>( static_cast<BASE>( rftl::numeric_limits<BASE>::max() - mod ) + 1 );

			// Safe, can't rollover twice
			BASE const adjustedOffset = static_cast<BASE>( rawOffset + spillover );

			// Safe, only reduces size
			BASE const wrapped = static_cast<BASE>( adjustedOffset % mod );
			return wrapped;
		}
		else
		{
			// Safe, only reduces size
			BASE const wrapped = static_cast<BASE>( rawOffset % mod );
			return wrapped;
		}
	}
	else
	{
		// TODO: Cast may not be safe in all cases, review
		OFFSET const minimalOffset = static_cast<OFFSET>( offset % mod );

		// TODO: Cast may not be safe in all cases, review
		BASE const positiveRollover = static_cast<BASE>( mod + minimalOffset );

		// Cast safe, see above assertion on unsigned rollover behavior
		BASE const rawOffset = static_cast<BASE>( value + positiveRollover );
		bool const overflow = rawOffset < value;
		if( overflow )
		{
			// Safe, mod can't exceed max
			BASE const spillover = static_cast<BASE>( static_cast<BASE>( rftl::numeric_limits<BASE>::max() - mod ) + 1 );

			// Safe, can't rollover twice
			BASE const adjustedOffset = static_cast<BASE>( rawOffset + spillover );

			// Safe, only reduces size
			BASE const wrapped = static_cast<BASE>( adjustedOffset % mod );
			return wrapped;
		}
		else
		{
			// Safe, only reduces size
			BASE const wrapped = static_cast<BASE>( rawOffset % mod );
			return wrapped;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}
