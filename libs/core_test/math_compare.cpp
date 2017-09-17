#include "stdafx.h"

#include "core_math/math_compare.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST(MathCompare, Basic)
{
	static_assert( Equals<float>( 0.f, 0.f ), "Zero equality failure" );
	static_assert( Equals<float>( 0.f, -0.f ), "Signed zero failure" );
	static_assert( Equals<float>( 1.f, 1.f ), "Basic equality failure" );
	static_assert( Equals<float>( 1.f, 1.f + std::numeric_limits<float>::epsilon() ), "Unsafe precision required" );
	static_assert( Equals<float>( 1.f, 1.0000001f ), "Too much precision required" );
	static_assert( Equals<float>( 1.f, 1.0000100f ) == false, "Too little precision required" );
	static_assert( Equals<float>( 10000000.f, 10000001.f ), "Too much precision required" );
	static_assert( Equals<float>( 10000000.f, 10000100.f ) == false, "Too little precision required" );

	static_assert( Equals<double>( 0.0, 0.0 ), "Zero equality failure" );
	static_assert( Equals<double>( 0.0, -0.0 ), "Signed zero failure" );
	static_assert( Equals<double>( 1.0, 1.0 ), "Basic equality failure" );
	static_assert( Equals<double>( 1.0, 1.0 + std::numeric_limits<double>::epsilon() ), "Unsafe precision required" );
	static_assert( Equals<double>( 1.0, 1.000000000001 ), "Too much precision required" );
	static_assert( Equals<double>( 1.0, 1.000000000100 ) == false, "Too little precision required" );
	static_assert( Equals<double>( 1000000000000.0, 1000000000001.0 ), "Too much precision required" );
	static_assert( Equals<double>( 1000000000000.0, 1000000000100.0 ) == false, "Too little precision required" );
}

///////////////////////////////////////////////////////////////////////////////
}}
