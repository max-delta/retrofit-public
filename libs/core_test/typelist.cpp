#include "stdafx.h"

#include "core/meta/TypeList.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( TypeList, Empty )
{
	// Should not cause compilation errors
	using tl_empty = TypeList<>;
	static_assert( tl_empty::kNumTypes == 0, "Unexpected size" );
	static_assert( tl_empty::Contains<void*>::value == false, "Unexpected type presence" );
	static_assert( tl_empty::FindIndex<void*>::value == -1, "Unexpected type presence" );
	static_assert( tl_empty::Reverse<>::type::kNumTypes == 0, "Unexpected size" );
}



TEST( TypeList, Single )
{
	// Should not cause logic errors in non-list usage
	using tl_f = TypeList<float>;
	static_assert( tl_f::kNumTypes == 1, "Unexpected size" );
	static_assert( std::is_same<tl_f::ByIndex<0>::type, float>::value, "Unexpected type" );
	static_assert( tl_f::Contains<void*>::value == false, "Unexpected type presence" );
	static_assert( tl_f::Contains<float>::value == true, "Unexpected type presence" );
	static_assert( tl_f::FindIndex<void*>::value == -1, "Unexpected type presence" );
	static_assert( tl_f::FindIndex<float>::value == 0, "Unexpected type presence" );
	static_assert( std::is_same<tl_f::Reverse<>::type::ByIndex<0>::type, float>::value, "Unexpected type" );
	static_assert( std::is_same<tl_f::Split<0>::latter::ByIndex<0>::type, float>::value, "Unexpected type" );
	static_assert( tl_f::Split<1>::latter::kNumTypes == 0, "Unexpected size" );
}



TEST( TypeList, Multiple )
{
	using tl_ifc = TypeList<int, float, char>;
	static_assert( tl_ifc::kNumTypes == 3, "Unexpected size" );
	static_assert( std::is_same<tl_ifc::ByIndex<0>::type, int>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::ByIndex<1>::type, float>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::ByIndex<2>::type, char>::value, "Unexpected type" );
	static_assert( tl_ifc::Contains<void*>::value == false, "Unexpected type presence" );
	static_assert( tl_ifc::Contains<int>::value == true, "Unexpected type presence" );
	static_assert( tl_ifc::Contains<float>::value == true, "Unexpected type presence" );
	static_assert( tl_ifc::Contains<char>::value == true, "Unexpected type presence" );
	static_assert( tl_ifc::FindIndex<void*>::value == -1, "Unexpected type presence" );
	static_assert( tl_ifc::FindIndex<int>::value == 0, "Unexpected type presence" );
	static_assert( tl_ifc::FindIndex<float>::value == 1, "Unexpected type presence" );
	static_assert( tl_ifc::FindIndex<char>::value == 2, "Unexpected type presence" );
	static_assert( std::is_same<tl_ifc::Reverse<>::type::ByIndex<0>::type, char>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::Reverse<>::type::ByIndex<1>::type, float>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::Reverse<>::type::ByIndex<2>::type, int>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::Split<0>::latter::ByIndex<0>::type, int>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::Split<0>::latter::ByIndex<1>::type, float>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::Split<0>::latter::ByIndex<2>::type, char>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::Split<1>::latter::ByIndex<0>::type, float>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::Split<1>::latter::ByIndex<1>::type, char>::value, "Unexpected type" );
	static_assert( std::is_same<tl_ifc::Split<2>::latter::ByIndex<0>::type, char>::value, "Unexpected type" );
	static_assert( tl_ifc::Split<3>::latter::kNumTypes == 0, "Unexpected size" );
}



TEST( TypeList, Add )
{
	using tl_ifc = TypeList<int, float, char>;
	using tl_empty = TypeList<>;

	// Append/prepend on empty should be identical
	static_assert( std::is_same<tl_empty::Append<float>::type, tl_empty::Prepend<float>::type>::value, "Unexpected type" );

	using tl_added_f = tl_empty::Append<float>::type;
	using tl_added_if = tl_added_f::Prepend<int>::type;
	using tl_added_ifc = tl_added_if::Append<char>::type;
	static_assert( std::is_same<tl_ifc, tl_added_ifc>::value, "Unexpected type" );
}

///////////////////////////////////////////////////////////////////////////////
}
