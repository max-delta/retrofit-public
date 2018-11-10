#include "stdafx.h"

#include "core_math/ListOperations.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST( MathListOperations, Sum )
{
	{
		using values = ValueList<int>;
		static_assert( ListSum<int, values>::value == 0, "Bad math" );
	}
	{
		using values = ValueList<int, 1>;
		static_assert( ListSum<int, values>::value == 1, "Bad math" );
	}
	{
		using values = ValueList<int, 1, 3, 5>;
		static_assert( ListSum<int, values>::value == 1 + 3 + 5, "Bad math" );
	}
}



TEST( MathListOperations, Product )
{
	{
		using values = ValueList<int>;
		static_assert( ListProduct<int, values>::value == 1, "Bad math" );
	}
	{
		using values = ValueList<int, 7>;
		static_assert( ListProduct<int, values>::value == 7, "Bad math" );
	}
	{
		using values = ValueList<int, 2, 3, 5>;
		static_assert( ListProduct<int, values>::value == 2 * 3 * 5, "Bad math" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
