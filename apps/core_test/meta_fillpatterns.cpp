#include "stdafx.h"

#include "core/meta/FillPatterns.h"

#include "core_math/SoftFloat.h"
#include "core_unicode/CharConvert.h"


namespace RF::meta::fill {
///////////////////////////////////////////////////////////////////////////////

TEST( FillPatterns, Bad4Bytes )
{
	uint32_t const memory[2] = { kBad4Bytes, kBad4Bytes };
	uint32_t const& asInt = memory[0];
	float const& asFloat = *reinterpret_cast<float const*>( &asInt );
	float const& asMisalignedFloat = *reinterpret_cast<float const*>( reinterpret_cast<uint16_t const*>( &asInt ) + 1 );
	char const( &asChars )[4] = *reinterpret_cast<char const( * )[4]>( &asInt );

	// Can't be interpreted as null
	ASSERT_NE( asInt, static_cast<uintptr_t>( 0 ) );

	// Can't be interpreted as a good float
	ASSERT_TRUE( math::SoftFloat<float>::IsNaN( asFloat ) );
	ASSERT_TRUE( math::SoftFloat<float>::IsSignalingNaN( asFloat ) );
	ASSERT_TRUE( math::SoftFloat<float>::IsNaN( asMisalignedFloat ) );
	ASSERT_TRUE( math::SoftFloat<float>::IsSignalingNaN( asMisalignedFloat ) );

	// Can't be interpreted as ASCII
	for( char const& ch : asChars )
	{
		ASSERT_FALSE( unicode::IsValidAscii( ch ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
