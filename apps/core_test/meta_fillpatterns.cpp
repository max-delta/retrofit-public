#include "stdafx.h"

#include "core/meta/FillPatterns.h"

#include "core_math/SoftFloat.h"
#include "core_unicode/CharConvert.h"


namespace RF::meta::fill {
///////////////////////////////////////////////////////////////////////////////

TEST( FillPatterns, Bad4Bytes )
{
	uint32_t const patterns[] = {
		kBadPattern4_0,
		kBadPattern4_A,
		kBadPattern4_B,
		kBadPattern4_8,
		kBadPattern4_9 };

	for( uint32_t const& pattern : patterns )
	{
		// Stack up the pattern twice in a row, to allow for misalignment tests
		struct Memory
		{
			uint32_t m1;
			uint32_t m2;
		};
		Memory const memory = { pattern, pattern };

		// Can't be interpreted as null
		uint32_t const& asInt = memory.m1;
		ASSERT_NE( asInt, static_cast<uintptr_t>( 0 ) );

		// Can't be interpreted as a good float
		float const& asFloat = *reinterpret_cast<float const*>( &asInt );
		ASSERT_TRUE( math::SoftFloat<float>::IsNaN( asFloat ) );
		ASSERT_TRUE( math::SoftFloat<float>::IsSignalingNaN( asFloat ) );

		// ... even if misaligned
		float const& asMisalignedFloat = *reinterpret_cast<float const*>( reinterpret_cast<uint16_t const*>( &asInt ) + 1 );
		ASSERT_TRUE( math::SoftFloat<float>::IsNaN( asMisalignedFloat ) );
		ASSERT_TRUE( math::SoftFloat<float>::IsSignalingNaN( asMisalignedFloat ) );

		// Can't be interpreted as ASCII
		char const( &asChars )[4] = *reinterpret_cast<char const( * )[4]>( &asInt );
		for( char const& ch : asChars )
		{
			ASSERT_FALSE( unicode::IsValidAscii( ch ) );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}
