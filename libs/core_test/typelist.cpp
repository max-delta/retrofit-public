#include "stdafx.h"

#include "core/meta/TypeList.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( TypeList, Empty )
{
	// Should not cause compilation errors
	using tl_empty = TypeList<>;
	static_assert( tl_empty::kNumTypes == 0, "Unexpected size" );
}



TEST( TypeList, Single )
{
	// Should not cause logic errors in non-list usage
	using tl_f = TypeList<float>;
	static_assert( tl_f::kNumTypes == 1, "Unexpected size" );
	static_assert( std::is_same<tl_f::ByIndex<0>::Type, float>::value, "Unexpected type" );
}



TEST( TypeList, Multiple )
{
	using tl_ifc = TypeList<int, float, char>;
	static_assert( tl_ifc::kNumTypes == 3, "Unexpected size" );
	static_assert( std::is_same<tl_ifc::ByIndex<0>::Type, int>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::ByIndex<1>::Type, float>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::ByIndex<2>::Type, char>::value, "Unexpected type" );
}

///////////////////////////////////////////////////////////////////////////////
}
