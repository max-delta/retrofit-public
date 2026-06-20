#include "stdafx.h"

#include "rftl/extension/array_string.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( ArrayString, Basics )
{
	using namespace rftl::literals;

	static_assert(
		""_array ==
		rftl::array<char, 0>{} );
	static_assert(
		"a"_array ==
		rftl::array{ 'a' } );
	static_assert(
		"ab"_array ==
		rftl::array{ 'a', 'b' } );
	static_assert(
		"ab\0"_array ==
		rftl::array{ 'a', 'b', '\0' } );
	static_assert(
		"a\0b"_array ==
		rftl::array{ 'a', '\0', 'b' } );
	static_assert(
		"\0ab"_array ==
		rftl::array{ '\0', 'a', 'b' } );
}

///////////////////////////////////////////////////////////////////////////////
}
