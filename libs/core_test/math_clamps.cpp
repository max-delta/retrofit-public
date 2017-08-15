#include "stdafx.h"

#include "core_math/math_clamps.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST(MathClamps, MinInt)
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
}



TEST(MathClamps, MinFloat)
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
}


TEST(MathClamps, MinNegative)
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
}



TEST(MathClamps, MaxInt)
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
}



TEST(MathClamps, MaxFloat)
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
}


TEST(MathClamps, MaxNegative)
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
}



TEST(MathClamps, ClampInt)
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



TEST(MathClamps, ClampFloat)
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



TEST(MathClamps, ClampNegative)
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

///////////////////////////////////////////////////////////////////////////////
}}
