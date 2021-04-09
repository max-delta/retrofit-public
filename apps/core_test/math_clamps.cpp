#include "stdafx.h"

#include "core_math/math_clamps.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

TEST( MathClamps, MinInt )
{
	{
		constexpr int min = Min( 5, 2 );
		static_assert( min == 2, "" );
		ASSERT_EQ( min, 2 );
	}
	{
		constexpr int min = Min( 2, 5 );
		static_assert( min == 2, "" );
		ASSERT_EQ( min, 2 );
	}
	{
		constexpr int min = Min( 2, 2 );
		static_assert( min == 2, "" );
		ASSERT_EQ( min, 2 );
	}
	{
		constexpr int min = Min( 2, 2, 1 );
		static_assert( min == 1, "" );
		ASSERT_EQ( min, 1 );
	}
}



TEST( MathClamps, MinFloat )
{
	{
		constexpr float min = Min( 5.f, 2.f );
		static_assert( min == 2.f, "" );
		ASSERT_EQ( min, 2.f );
	}
	{
		constexpr float min = Min( 2.f, 5.f );
		static_assert( min == 2.f, "" );
		ASSERT_EQ( min, 2.f );
	}
	{
		constexpr float min = Min( 2.f, 2.f );
		static_assert( min == 2.f, "" );
		ASSERT_EQ( min, 2.f );
	}
	{
		constexpr float min = Min( 2.f, 2.f, 1.f );
		static_assert( min == 1.f, "" );
		ASSERT_EQ( min, 1.f );
	}
}


TEST( MathClamps, MinNegative )
{
	{
		constexpr int min = Min( -7, 7 );
		static_assert( min == -7, "" );
		ASSERT_EQ( min, -7 );
	}
	{
		constexpr int min = Min( 7, -7 );
		static_assert( min == -7, "" );
		ASSERT_EQ( min, -7 );
	}
	{
		constexpr int min = Min( -5, -7 );
		static_assert( min == -7, "" );
		ASSERT_EQ( min, -7 );
	}
	{
		constexpr int min = Min( -7, -5 );
		static_assert( min == -7, "" );
		ASSERT_EQ( min, -7 );
	}
	{
		constexpr int min = Min( -7, -5, -9 );
		static_assert( min == -9, "" );
		ASSERT_EQ( min, -9 );
	}
}



TEST( MathClamps, MaxInt )
{
	{
		constexpr int max = Max( 5, 2 );
		static_assert( max == 5, "" );
		ASSERT_EQ( max, 5 );
	}
	{
		constexpr int max = Max( 2, 5 );
		static_assert( max == 5, "" );
		ASSERT_EQ( max, 5 );
	}
	{
		constexpr int max = Max( 2, 2 );
		static_assert( max == 2, "" );
		ASSERT_EQ( max, 2 );
	}
	{
		constexpr int max = Max( 2, 2, 5 );
		static_assert( max == 5, "" );
		ASSERT_EQ( max, 5 );
	}
}



TEST( MathClamps, MaxFloat )
{
	{
		constexpr float max = Max( 5.f, 2.f );
		static_assert( max == 5.f, "" );
		ASSERT_EQ( max, 5.f );
	}
	{
		constexpr float max = Max( 2.f, 5.f );
		static_assert( max == 5.f, "" );
		ASSERT_EQ( max, 5.f );
	}
	{
		constexpr float max = Max( 2.f, 2.f );
		static_assert( max == 2.f, "" );
		ASSERT_EQ( max, 2.f );
	}
	{
		constexpr float max = Max( 2.f, 5.f );
		static_assert( max == 5.f, "" );
		ASSERT_EQ( max, 5.f );
	}
}


TEST( MathClamps, MaxNegative )
{
	{
		constexpr int max = Max( -7, 7 );
		static_assert( max == 7, "" );
		ASSERT_EQ( max, 7 );
	}
	{
		constexpr int max = Max( 7, -7 );
		static_assert( max == 7, "" );
		ASSERT_EQ( max, 7 );
	}
	{
		constexpr int max = Max( -5, -7 );
		static_assert( max == -5, "" );
		ASSERT_EQ( max, -5 );
	}
	{
		constexpr int max = Max( -7, -5 );
		static_assert( max == -5, "" );
		ASSERT_EQ( max, -5 );
	}
	{
		constexpr int max = Max( -7, -5, -3 );
		static_assert( max == -3, "" );
		ASSERT_EQ( max, -3 );
	}
}



TEST( MathClamps, ClampInt )
{
	{
		constexpr int clamp = Clamp( 2, 5, 10 );
		static_assert( clamp == 5, "" );
		ASSERT_EQ( clamp, 5 );
	}
	{
		constexpr int clamp = Clamp( 2, 15, 10 );
		static_assert( clamp == 10, "" );
		ASSERT_EQ( clamp, 10 );
	}
	{
		constexpr int clamp = Clamp( 2, 1, 10 );
		static_assert( clamp == 2, "" );
		ASSERT_EQ( clamp, 2 );
	}
	{
		constexpr int clamp = Clamp( 2, 2, 2 );
		static_assert( clamp == 2, "" );
		ASSERT_EQ( clamp, 2 );
	}
}



TEST( MathClamps, ClampFloat )
{
	{
		constexpr float clamp = Clamp( 2.f, 5.f, 10.f );
		static_assert( clamp == 5.f, "" );
		ASSERT_EQ( clamp, 5.f );
	}
	{
		constexpr float clamp = Clamp( 2.f, 15.f, 10.f );
		static_assert( clamp == 10.f, "" );
		ASSERT_EQ( clamp, 10.f );
	}
	{
		constexpr float clamp = Clamp( 2.f, 1.f, 10.f );
		static_assert( clamp == 2.f, "" );
		ASSERT_EQ( clamp, 2.f );
	}
	{
		constexpr float clamp = Clamp( 2.f, 2.f, 2.f );
		static_assert( clamp == 2.f, "" );
		ASSERT_EQ( clamp, 2.f );
	}
}



TEST( MathClamps, ClampNegative )
{
	{
		constexpr int clamp = Clamp( -10, -5, -2 );
		static_assert( clamp == -5, "" );
		ASSERT_EQ( clamp, -5 );
	}
	{
		constexpr int clamp = Clamp( -10, -15, -2 );
		static_assert( clamp == -10, "" );
		ASSERT_EQ( clamp, -10 );
	}
	{
		constexpr int clamp = Clamp( -10, -1, -2 );
		static_assert( clamp == -2, "" );
		ASSERT_EQ( clamp, -2 );
	}
	{
		constexpr int clamp = Clamp( -2, -2, -2 );
		static_assert( clamp == -2, "" );
		ASSERT_EQ( clamp, -2 );
	}
}



TEST( MathClamps, SnapNearest )
{
	// TODO: Floating point support and tests
	static_assert( SnapNearest( -5, 4 ) == -4, "" );
	static_assert( SnapNearest( -4, 4 ) == -4, "" );
	static_assert( SnapNearest( -3, 4 ) == -4, "" );
	static_assert( SnapNearest( -2, 4 ) == 0, "" );
	static_assert( SnapNearest( -1, 4 ) == 0, "" );
	static_assert( SnapNearest( 0, 4 ) == 0, "" );
	static_assert( SnapNearest( 1, 4 ) == 0, "" );
	static_assert( SnapNearest( 2, 4 ) == 0, "" );
	static_assert( SnapNearest( 3, 4 ) == 4, "" );
	static_assert( SnapNearest( 4, 4 ) == 4, "" );
	static_assert( SnapNearest( 5, 4 ) == 4, "" );

	// Uint8_t's get weird promotion rules, so take some extra casting
	static_assert( SnapNearest<uint8_t>( 0, 4 ) == 0, "" );
	static_assert( SnapNearest<uint8_t>( 1, 4 ) == 0, "" );
	static_assert( SnapNearest<uint8_t>( 2, 4 ) == 0, "" );
	static_assert( SnapNearest<uint8_t>( 3, 4 ) == 4, "" );
	static_assert( SnapNearest<uint8_t>( 4, 4 ) == 4, "" );
	static_assert( SnapNearest<uint8_t>( 5, 4 ) == 4, "" );
}



TEST( MathClamps, SnapHighest )
{
	// TODO: Floating point support and tests
	static_assert( SnapHighest( -5, 4 ) == -4, "" );
	static_assert( SnapHighest( -4, 4 ) == -4, "" );
	static_assert( SnapHighest( -3, 4 ) == 0, "" );
	static_assert( SnapHighest( -2, 4 ) == 0, "" );
	static_assert( SnapHighest( -1, 4 ) == 0, "" );
	static_assert( SnapHighest( 0, 4 ) == 0, "" );
	static_assert( SnapHighest( 1, 4 ) == 4, "" );
	static_assert( SnapHighest( 2, 4 ) == 4, "" );
	static_assert( SnapHighest( 3, 4 ) == 4, "" );
	static_assert( SnapHighest( 4, 4 ) == 4, "" );
	static_assert( SnapHighest( 5, 4 ) == 8, "" );

	// Uint8_t's get weird promotion rules, so take some extra casting
	static_assert( SnapHighest<uint8_t>( 0, 4 ) == 0, "" );
	static_assert( SnapHighest<uint8_t>( 1, 4 ) == 4, "" );
	static_assert( SnapHighest<uint8_t>( 2, 4 ) == 4, "" );
	static_assert( SnapHighest<uint8_t>( 3, 4 ) == 4, "" );
	static_assert( SnapHighest<uint8_t>( 4, 4 ) == 4, "" );
	static_assert( SnapHighest<uint8_t>( 5, 4 ) == 8, "" );
}



TEST( MathClamps, SnapLowest )
{
	// TODO: Floating point support and tests
	static_assert( SnapLowest( -5, 4 ) == -8, "" );
	static_assert( SnapLowest( -4, 4 ) == -4, "" );
	static_assert( SnapLowest( -3, 4 ) == -4, "" );
	static_assert( SnapLowest( -2, 4 ) == -4, "" );
	static_assert( SnapLowest( -1, 4 ) == -4, "" );
	static_assert( SnapLowest( 0, 4 ) == 0, "" );
	static_assert( SnapLowest( 1, 4 ) == 0, "" );
	static_assert( SnapLowest( 2, 4 ) == 0, "" );
	static_assert( SnapLowest( 3, 4 ) == 0, "" );
	static_assert( SnapLowest( 4, 4 ) == 4, "" );
	static_assert( SnapLowest( 5, 4 ) == 4, "" );

	// Uint8_t's get weird promotion rules, so take some extra casting
	static_assert( SnapLowest<uint8_t>( 0, 4 ) == 0, "" );
	static_assert( SnapLowest<uint8_t>( 1, 4 ) == 0, "" );
	static_assert( SnapLowest<uint8_t>( 2, 4 ) == 0, "" );
	static_assert( SnapLowest<uint8_t>( 3, 4 ) == 0, "" );
	static_assert( SnapLowest<uint8_t>( 4, 4 ) == 4, "" );
	static_assert( SnapLowest<uint8_t>( 5, 4 ) == 4, "" );
}



TEST( MathClamps, WrapPositiveOffset )
{
	// TODO: Floating point support and tests

	// Uint8_t's get weird promotion rules, so take some extra casting
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, -3 ) == 1, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, -2 ) == 2, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, -1 ) == 0, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, 0 ) == 1, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, 1 ) == 2, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, 2 ) == 0, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, 3 ) == 1, "" );

	// Unwrapped offset values are permitted
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, -127 ) == 0, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, -126 ) == 1, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, 126 ) == 1, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 1, 3, 127 ) == 2, "" );

	// Unwrapped initial values are permitted
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 127, 3, 0 ) == 1, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 127, 3, 1 ) == 2, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 127, 3, 2 ) == 0, "" );

	// Unwrapped offset+initial values are permitted
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 127, 3, -127 ) == 0, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 127, 3, -126 ) == 1, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 127, 3, 126 ) == 1, "" );
	static_assert( WrapPositiveOffset<uint8_t, int8_t>( 127, 3, 127 ) == 2, "" );

	// Overflows for values with mods that don't align to the max integer value
	static_assert( WrapPositiveOffset<uint8_t, uint8_t>( 252, 254, 0 ) == 252, "" );
	static_assert( WrapPositiveOffset<uint8_t, uint8_t>( 252, 254, 1 ) == 253, "" );
	static_assert( WrapPositiveOffset<uint8_t, uint8_t>( 252, 254, 2 ) == 0, "" );
	static_assert( WrapPositiveOffset<uint8_t, uint8_t>( 252, 254, 3 ) == 1, "" );
	static_assert( WrapPositiveOffset<uint8_t, uint8_t>( 252, 254, 4 ) == 2, "" );
}



TEST( MathClamps, AbsInt )
{
	{
		constexpr int abs = Abs( 5 );
		static_assert( abs == 5 );
	}
	{
		constexpr int abs = Abs( -5 );
		static_assert( abs == 5 );
	}
	{
		constexpr int abs = Abs( rftl::numeric_limits<int>::min() );
		static_assert( abs == rftl::numeric_limits<int>::max() );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
