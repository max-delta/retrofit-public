#include "stdafx.h"

#include "core_math/Ratio.h"

#include "rftl/extension/integer_literals.h"
#include "rftl/limits"
#include "rftl/cmath"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

TEST( Ratio, Validity )
{
	ASSERT_FALSE( Ratio8( 0, 0 ).IsValid() );
	ASSERT_FALSE( Ratio8( 1, 0 ).IsValid() );

	ASSERT_FALSE( Ratio16( 0, 0 ).IsValid() );
	ASSERT_FALSE( Ratio16( 1, 0 ).IsValid() );

	ASSERT_FALSE( Ratio32( 0, 0 ).IsValid() );
	ASSERT_FALSE( Ratio32( 1, 0 ).IsValid() );
}



TEST( Ratio, Components )
{
	using namespace rftl::literals;

	// Invalid
	ASSERT_EQ( Ratio8( 0, 0 ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( Ratio16( 0, 0 ).GetAsPair(), Ratio16::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( Ratio32( 0, 0 ).GetAsPair(), Ratio32::Pair( 0_u16, 0_u16 ) );

	// Smallest storable non-zero sizes
	ASSERT_EQ( Ratio8( 1, 15 ).GetAsPair(), Ratio8::Pair( 1_u8, 15_u8 ) );
	ASSERT_EQ( Ratio16( 1, 255 ).GetAsPair(), Ratio16::Pair( 1_u8, 255_u8 ) );
	ASSERT_EQ( Ratio32( 1, 65535 ).GetAsPair(), Ratio32::Pair( 1_u16, 65535_u16 ) );

	// Largest storable sizes
	ASSERT_EQ( Ratio8( 15, 1 ).GetAsPair(), Ratio8::Pair( 15_u8, 1_u8 ) );
	ASSERT_EQ( Ratio16( 255, 1 ).GetAsPair(), Ratio16::Pair( 255_u8, 1_u8 ) );
	ASSERT_EQ( Ratio32( 65535, 1 ).GetAsPair(), Ratio32::Pair( 65535_u16, 1_u16 ) );
}



TEST( Ratio, Floats )
{
	using namespace rftl::literals;

	// Invalid
	ASSERT_TRUE( rftl::isnan( Ratio8( 0, 0 ).GetAsFloat() ) );
	ASSERT_TRUE( rftl::isnan( Ratio16( 0, 0 ).GetAsFloat() ) );
	ASSERT_TRUE( rftl::isnan( Ratio32( 0, 0 ).GetAsFloat() ) );

	// Smallest storable non-zero sizes
	// NOTE: Not cleanly representable by floats
	static constexpr float kPrecision = 0.0000001f;
	ASSERT_NEAR( Ratio8( 1, 15 ).GetAsFloat(), 1.f / 15.f, kPrecision );
	ASSERT_NEAR( Ratio16( 1, 255 ).GetAsFloat(), 1.f / 255.f, kPrecision );
	ASSERT_NEAR( Ratio32( 1, 65535 ).GetAsFloat(), 1.f / 65535.f, kPrecision );

	// Largest storable sizes
	ASSERT_EQ( Ratio8( 15, 1 ).GetAsFloat(), 15.f );
	ASSERT_EQ( Ratio16( 255, 1 ).GetAsFloat(), 255.f );
	ASSERT_EQ( Ratio32( 65535, 1 ).GetAsFloat(), 65535.f );
}



TEST( Ratio, Truncate )
{
	using namespace rftl::literals;

	// Smallest storable non-zero sizes
	ASSERT_EQ( Ratio8( 1, 15 ).GetAsIntegerViaTruncation(), 0u );
	ASSERT_EQ( Ratio16( 1, 255 ).GetAsIntegerViaTruncation(), 0u );
	ASSERT_EQ( Ratio32( 1, 65535 ).GetAsIntegerViaTruncation(), 0u );

	// One and three-quarters
	ASSERT_EQ( Ratio8( 7, 4 ).GetAsIntegerViaTruncation(), 1u );
	ASSERT_EQ( Ratio16( 7, 4 ).GetAsIntegerViaTruncation(), 1u );
	ASSERT_EQ( Ratio32( 7, 4 ).GetAsIntegerViaTruncation(), 1u );

	// Largest storable sizes
	ASSERT_EQ( Ratio8( 15, 1 ).GetAsIntegerViaTruncation(), 15u );
	ASSERT_EQ( Ratio16( 255, 1 ).GetAsIntegerViaTruncation(), 255u );
	ASSERT_EQ( Ratio32( 65535, 1 ).GetAsIntegerViaTruncation(), 65535u );
}



TEST( Ratio, Simplify )
{
	using namespace rftl::literals;

	// Invalid
	ASSERT_EQ( Ratio8( 0, 0 ).Simplify().GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( Ratio8( 1, 0 ).Simplify().GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );

	// Can't simplify
	ASSERT_EQ( Ratio8( 1, 15 ).Simplify().GetAsPair(), Ratio8::Pair( 1_u8, 15_u8 ) );
	ASSERT_EQ( Ratio8( 2, 3 ).Simplify().GetAsPair(), Ratio8::Pair( 2_u8, 3_u8 ) );
	ASSERT_EQ( Ratio8( 15, 1 ).Simplify().GetAsPair(), Ratio8::Pair( 15_u8, 1_u8 ) );

	// Can simplify
	ASSERT_EQ( Ratio8( 7, 14 ).Simplify().GetAsPair(), Ratio8::Pair( 1_u8, 2_u8 ) );
	ASSERT_EQ( Ratio8( 8, 12 ).Simplify().GetAsPair(), Ratio8::Pair( 2_u8, 3_u8 ) );
	ASSERT_EQ( Ratio8( 14, 2 ).Simplify().GetAsPair(), Ratio8::Pair( 7_u8, 1_u8 ) );
}



TEST( Ratio, Compress )
{
	using namespace rftl::literals;

	using BitCounts = Ratio8::BitCounts;

	// Invalid
	ASSERT_EQ( Ratio8( 0, 0 ).GetBitsRequiredToRepresent(), BitCounts( 0u, 0u ) );
	ASSERT_EQ( Ratio8( 0, 0 ).TryLossyCompress( 1 ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( Ratio8( 15, 0 ).GetBitsRequiredToRepresent(), BitCounts( 4u, 0u ) );
	ASSERT_EQ( Ratio8( 15, 0 ).TryLossyCompress( 1 ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );

	// Zero
	ASSERT_EQ( Ratio8( 0, 1 ).GetBitsRequiredToRepresent(), BitCounts( 0u, 1u ) );
	ASSERT_EQ( Ratio8( 0, 1 ).TryLossyCompress( 3 ).GetAsPair(), Ratio8::Pair( 0_u8, 1_u8 ) );
	ASSERT_EQ( Ratio8( 0, 15 ).GetBitsRequiredToRepresent(), BitCounts( 0u, 4u ) );
	ASSERT_EQ( Ratio8( 0, 15 ).TryLossyCompress( 3 ).GetAsPair(), Ratio8::Pair( 0_u8, 1_u8 ) );

	// Integer, can't compress
	ASSERT_EQ( Ratio8( 1, 1 ).GetBitsRequiredToRepresent(), BitCounts( 1u, 1u ) );
	ASSERT_EQ( Ratio8( 1, 1 ).TryLossyCompress( 1 ).GetAsPair(), Ratio8::Pair( 1_u8, 1_u8 ) );
	ASSERT_EQ( Ratio8( 15, 1 ).GetBitsRequiredToRepresent(), BitCounts( 4u, 1u ) );
	ASSERT_EQ( Ratio8( 15, 1 ).TryLossyCompress( 1 ).GetAsPair(), Ratio8::Pair( 15_u8, 1_u8 ) );

	// Min fraction, can't compress
	ASSERT_EQ( Ratio8( 1, 2 ).GetBitsRequiredToRepresent(), BitCounts( 1u, 2u ) );
	ASSERT_EQ( Ratio8( 1, 2 ).TryLossyCompress( 1 ).GetAsPair(), Ratio8::Pair( 1_u8, 2_u8 ) );
	ASSERT_EQ( Ratio8( 1, 15 ).GetBitsRequiredToRepresent(), BitCounts( 1u, 4u ) );
	ASSERT_EQ( Ratio8( 1, 15 ).TryLossyCompress( 1 ).GetAsPair(), Ratio8::Pair( 1_u8, 15_u8 ) );

	// Trivial simplify better than requested
	ASSERT_EQ( Ratio8( 2, 4 ).GetBitsRequiredToRepresent(), BitCounts( 2u, 3u ) );
	ASSERT_EQ( Ratio8( 2, 4 ).TryLossyCompress( 3 ).GetAsPair(), Ratio8::Pair( 1_u8, 2_u8 ) );

	// Can't simplify to requested, but COULD compress
	ASSERT_EQ( Ratio8( 5, 6 ).GetBitsRequiredToRepresent(), BitCounts( 3u, 3u ) );
	ASSERT_EQ( Ratio8( 5, 6 ).TryLossyCompress( 3 ).GetAsPair(), Ratio8::Pair( 5_u8, 6_u8 ) );
	ASSERT_EQ( Ratio8( 6, 7 ).GetBitsRequiredToRepresent(), BitCounts( 3u, 3u ) );
	ASSERT_EQ( Ratio8( 6, 7 ).TryLossyCompress( 3 ).GetAsPair(), Ratio8::Pair( 6_u8, 7_u8 ) );

	// Can compress, but not simplify further
	ASSERT_EQ( Ratio8( 5, 6 ).GetBitsRequiredToRepresent(), BitCounts( 3u, 3u ) );
	ASSERT_EQ( Ratio8( 5, 6 ).TryLossyCompress( 2 ).GetAsPair(), Ratio8::Pair( 2_u8, 3_u8 ) );

	// Can compress, and simplify further
	ASSERT_EQ( Ratio8( 6, 7 ).GetBitsRequiredToRepresent(), BitCounts( 3u, 3u ) );
	ASSERT_EQ( Ratio8( 6, 7 ).TryLossyCompress( 2 ).GetAsPair(), Ratio8::Pair( 1_u8, 1_u8 ) );
}



TEST( Ratio, Upsize )
{
	using namespace rftl::literals;

	// Invalid
	ASSERT_EQ( static_cast<Ratio16>( Ratio8( 0, 0 ) ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( static_cast<Ratio32>( Ratio16( 0, 0 ) ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );

	// Smallest storable non-zero sizes
	ASSERT_EQ( static_cast<Ratio16>( Ratio8( 1, 15 ) ).GetAsPair(), Ratio8::Pair( 1_u8, 15_u8 ) );
	ASSERT_EQ( static_cast<Ratio32>( Ratio16( 1, 255 ) ).GetAsPair(), Ratio16::Pair( 1_u8, 255_u8 ) );

	// Largest storable sizes
	ASSERT_EQ( static_cast<Ratio16>( Ratio8( 15, 1 ) ).GetAsPair(), Ratio8::Pair( 15_u8, 1_u8 ) );
	ASSERT_EQ( static_cast<Ratio32>( Ratio16( 255, 1 ) ).GetAsPair(), Ratio16::Pair( 255_u8, 1_u8 ) );
}



TEST( Ratio, CompareInt )
{
	using namespace rftl::literals;

	ASSERT_FALSE( Ratio8( 0, 0 ) < 0_u8 );
	ASSERT_TRUE( Ratio8( 0, 0 ) <= 0_u8 );
	ASSERT_TRUE( Ratio8( 0, 0 ) == 0_u8 );
	ASSERT_TRUE( Ratio8( 0, 0 ) >= 0_u8 );
	ASSERT_FALSE( Ratio8( 0, 0 ) > 0_u8 );

	Ratio8 const zero( 0, 1 );
	ASSERT_FALSE( zero < 0_u8 );
	ASSERT_TRUE( zero < 1_u8 );
	ASSERT_TRUE( zero <= 0_u8 );
	ASSERT_FALSE( zero == 1_u8 );
	ASSERT_TRUE( zero == 0_u8 );
	ASSERT_TRUE( zero >= 0_u8 );
	ASSERT_FALSE( zero > 0_u8 );

	Ratio8 const half( 1, 2 );
	ASSERT_FALSE( half < 0_u8 );
	ASSERT_TRUE( half < 1_u8 );
	ASSERT_FALSE( half <= 0_u8 );
	ASSERT_TRUE( half <= 1_u8 );
	ASSERT_FALSE( half == 0_u8 );
	ASSERT_TRUE( half >= 0_u8 );
	ASSERT_TRUE( half > 0_u8 );

	Ratio8 const one( 2, 2 );
	ASSERT_FALSE( one < 1_u8 );
	ASSERT_TRUE( one < 2_u8 );
	ASSERT_FALSE( one <= 0_u8 );
	ASSERT_TRUE( one <= 1_u8 );
	ASSERT_FALSE( one == 0_u8 );
	ASSERT_TRUE( one == 1_u8 );
	ASSERT_FALSE( one >= 2_u8 );
	ASSERT_TRUE( one >= 1_u8 );
	ASSERT_FALSE( one > 1_u8 );
	ASSERT_TRUE( one > 0_u8 );

	Ratio8 const onePointFive( 3, 2 );
	ASSERT_FALSE( onePointFive < 1_u8 );
	ASSERT_TRUE( onePointFive < 2_u8 );
	ASSERT_FALSE( onePointFive <= 1_u8 );
	ASSERT_TRUE( onePointFive <= 2_u8 );
	ASSERT_FALSE( onePointFive == 1_u8 );
	ASSERT_FALSE( onePointFive >= 2_u8 );
	ASSERT_TRUE( onePointFive >= 1_u8 );
	ASSERT_FALSE( onePointFive > 2_u8 );
	ASSERT_TRUE( onePointFive > 1_u8 );

	Ratio8 const two( 4, 2 );
	ASSERT_FALSE( two < 2_u8 );
	ASSERT_TRUE( two < 3_u8 );
	ASSERT_FALSE( two <= 1_u8 );
	ASSERT_TRUE( two <= 2_u8 );
	ASSERT_FALSE( two == 0_u8 );
	ASSERT_TRUE( two == 2_u8 );
	ASSERT_FALSE( two >= 3_u8 );
	ASSERT_TRUE( two >= 2_u8 );
	ASSERT_FALSE( two > 2_u8 );
	ASSERT_TRUE( two > 1_u8 );
}



TEST( Ratio, CompareRatio )
{
	ASSERT_TRUE( Ratio8( 0, 0 ) == Ratio8( 0, 0 ) );

	ASSERT_TRUE( Ratio8( 0, 1 ) == Ratio8( 0, 2 ) );
	ASSERT_TRUE( Ratio8( 1, 2 ) == Ratio8( 2, 4 ) );

	ASSERT_TRUE( Ratio8( 1, 2 ) < Ratio8( 3, 4 ) );
	ASSERT_TRUE( Ratio8( 8, 7 ) > Ratio8( 15, 15 ) );
}



TEST( Ratio, MathOps )
{
	using namespace rftl::literals;

	// Because GTest loses its shit when it sees a shift operator
	static auto constexpr shift = []( auto lhs, auto rhs ) -> auto
	{
		return lhs >> rhs;
	};

	// Invalid
	ASSERT_EQ( ( Ratio8( 0, 0 ) + Ratio8( 0, 0 ) ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( ( Ratio8( 0, 0 ) - Ratio8( 0, 0 ) ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( ( Ratio8( 0, 0 ) * Ratio8( 0, 0 ) ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( ( Ratio8( 0, 0 ) / Ratio8( 0, 0 ) ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );
	ASSERT_EQ( shift( Ratio8( 0, 0 ), 0u ).GetAsPair(), Ratio8::Pair( 0_u8, 0_u8 ) );

	// Plus
	ASSERT_EQ( ( Ratio8( 1, 3 ) + Ratio8( 1, 3 ) ).GetAsPair(), Ratio8::Pair( 2_u8, 3_u8 ) );
	ASSERT_EQ( ( Ratio8( 2, 3 ) + Ratio8( 1, 4 ) ).GetAsPair(), Ratio8::Pair( 11_u8, 12_u8 ) );
	ASSERT_EQ( ( Ratio8( 1, 15 ) + Ratio8( 1, 15 ) ).GetAsPair(), Ratio8::Pair( 2_u8, 15_u8 ) );

	// Minus
	ASSERT_EQ( ( Ratio8( 1, 3 ) - Ratio8( 1, 3 ) ).GetAsPair(), Ratio8::Pair( 0_u8, 1_u8 ) );
	ASSERT_EQ( ( Ratio8( 2, 3 ) - Ratio8( 1, 3 ) ).GetAsPair(), Ratio8::Pair( 1_u8, 3_u8 ) );
	ASSERT_EQ( ( Ratio8( 2, 3 ) - Ratio8( 1, 4 ) ).GetAsPair(), Ratio8::Pair( 5_u8, 12_u8 ) );
	ASSERT_EQ( ( Ratio8( 2, 15 ) - Ratio8( 1, 15 ) ).GetAsPair(), Ratio8::Pair( 1_u8, 15_u8 ) );

	// Multiply
	ASSERT_EQ( ( Ratio8( 1, 3 ) * Ratio8( 1, 3 ) ).GetAsPair(), Ratio8::Pair( 1_u8, 9_u8 ) );
	ASSERT_EQ( ( Ratio8( 2, 3 ) * Ratio8( 5, 6 ) ).GetAsPair(), Ratio8::Pair( 5_u8, 9_u8 ) );
	ASSERT_EQ( ( Ratio8( 15, 1 ) * Ratio8( 1, 1 ) ).GetAsPair(), Ratio8::Pair( 15_u8, 1_u8 ) );

	// Divide
	ASSERT_EQ( ( Ratio8( 1, 3 ) / Ratio8( 1, 3 ) ).GetAsPair(), Ratio8::Pair( 1_u8, 1_u8 ) );
	ASSERT_EQ( ( Ratio8( 2, 3 ) / Ratio8( 5, 6 ) ).GetAsPair(), Ratio8::Pair( 4_u8, 5_u8 ) );
	ASSERT_EQ( ( Ratio8( 15, 1 ) / Ratio8( 1, 1 ) ).GetAsPair(), Ratio8::Pair( 15_u8, 1_u8 ) );

	// Shift
	ASSERT_EQ( shift( Ratio8( 4, 8 ), 1u ).GetAsPair(), Ratio8::Pair( 2_u8, 4_u8 ) );
	ASSERT_EQ( shift( Ratio8( 2, 3 ), 1u ).GetAsPair(), Ratio8::Pair( 1_u8, 1_u8 ) );
	ASSERT_EQ( shift( Ratio8( 1, 15 ), 1u ).GetAsPair(), Ratio8::Pair( 0_u8, 7_u8 ) );
}

///////////////////////////////////////////////////////////////////////////////
}
