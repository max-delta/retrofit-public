#include "stdafx.h"

#include "rftl/extension/algorithms.h"

#include "rftl/string_view"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( Algorithms, PartitionPointIter )
{
	static constexpr auto checkForA = []( rftl::string_view::const_iterator iter ) -> bool
	{
		return *iter == 'A';
	};

	static constexpr auto indexAfterLastA = []( rftl::string_view str ) -> ptrdiff_t
	{
		rftl::string_view::const_iterator const iter =
			rftl::partition_point_iter( str.begin(), str.end(), checkForA );
		return iter - str.begin();
	};

	ASSERT_EQ( indexAfterLastA( "" ), 0 );
	ASSERT_EQ( indexAfterLastA( "B" ), 0 );
	ASSERT_EQ( indexAfterLastA( "A" ), 1 );
	ASSERT_EQ( indexAfterLastA( "AB" ), 1 );
	ASSERT_EQ( indexAfterLastA( "AA" ), 2 );
	ASSERT_EQ( indexAfterLastA( "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB" ), 67 );
}

///////////////////////////////////////////////////////////////////////////////
}
