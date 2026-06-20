#include "stdafx.h"

#include "rftl/extension/array_concat.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( ArrayConcat, Basics )
{
	static_assert(
		rftl::array_concat<int>() ==
		rftl::array<int, 0>{} );

	static_assert(
		rftl::array_concat( rftl::array{ 1 } ) ==
		rftl::array{ 1 } );

	static_assert(
		rftl::array_concat( rftl::array{ 1, 2 } ) ==
		rftl::array{ 1, 2 } );
	static_assert(
		rftl::array_concat( rftl::array{ 1 }, rftl::array{ 2 } ) ==
		rftl::array{ 1, 2 } );

	static_assert(
		rftl::array_concat( rftl::array{ 1 }, rftl::array{ 2 }, rftl::array{ 3 } ) ==
		rftl::array{ 1, 2, 3 } );
	static_assert(
		rftl::array_concat( rftl::array{ 1 }, rftl::array{ 2, 3 } ) ==
		rftl::array{ 1, 2, 3 } );
	static_assert(
		rftl::array_concat( rftl::array{ 1, 2 }, rftl::array{ 3 } ) ==
		rftl::array{ 1, 2, 3 } );
}

///////////////////////////////////////////////////////////////////////////////
}
