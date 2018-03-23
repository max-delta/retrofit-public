#include "stdafx.h"

#include "core/meta/ValueList.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( ValueList, Empty )
{
	// Should not cause compilation errors
	using tl_empty = ValueList<int>;
	static_assert( tl_empty::kNumValues == 0, "Unexpected size" );
	static_assert( tl_empty::Contains<5>::value == false, "Unexpected value presence" );
	static_assert( tl_empty::FindIndex<5>::value == -1, "Unexpected value presence" );
	static_assert( tl_empty::Reverse<>::type::kNumValues == 0, "Unexpected size" );
	static_assert( tl_empty::Split<0>::former::kNumValues == 0, "Unexpected size" );
	static_assert( tl_empty::Split<0>::latter::kNumValues == 0, "Unexpected size" );
}



TEST( ValueList, UnorthodoxValueTypes )
{
	// While ill-advised, should not cause compilation errors
	struct Unused;
	using tl_empty = ValueList<Unused*>;
	static_assert( tl_empty::kNumValues == 0, "Unexpected size" );
	#ifndef __INTELLISENSE__ // Intellisense falls over in MSVC2017
		static_assert( tl_empty::Contains<reinterpret_cast<Unused*>( 5 )>::value == false, "Unexpected value presence" );
		static_assert( tl_empty::FindIndex<reinterpret_cast<Unused*>( 5 )>::value == -1, "Unexpected value presence" );
		static_assert( tl_empty::Reverse<>::type::kNumValues == 0, "Unexpected size" );
		static_assert( tl_empty::Split<0>::former::kNumValues == 0, "Unexpected size" );
		static_assert( tl_empty::Split<0>::latter::kNumValues == 0, "Unexpected size" );
	#endif
}



TEST( ValueList, Single )
{
	// Should not cause logic errors in non-list usage
	using tl_f = ValueList<int, 5>;
	static_assert( tl_f::kNumValues == 1, "Unexpected size" );
	static_assert( tl_f::ByIndex<0>::value == 5, "Unexpected value" );
	static_assert( tl_f::Contains<3>::value == false, "Unexpected value presence" );
	static_assert( tl_f::Contains<5>::value == true, "Unexpected value presence" );
	static_assert( tl_f::FindIndex<3>::value == -1, "Unexpected value presence" );
	static_assert( tl_f::FindIndex<5>::value == 0, "Unexpected value presence" );
	static_assert( tl_f::Reverse<>::type::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( tl_f::Split<0>::former::kNumValues == 0, "Unexpected size" );
	static_assert( tl_f::Split<0>::latter::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( tl_f::Split<1>::former::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( tl_f::Split<1>::latter::kNumValues == 0, "Unexpected size" );
}



TEST( ValueList, Multiple )
{
	using tl_ifc = ValueList<int, 5, 7, 8>;
	static_assert( tl_ifc::kNumValues == 3, "Unexpected size" );
	static_assert( tl_ifc::ByIndex<0>::value == 5, "Unexpected value" );
	static_assert( tl_ifc::ByIndex<1>::value == 7, "Unexpected value" );
	static_assert( tl_ifc::ByIndex<2>::value == 8, "Unexpected value" );
	static_assert( tl_ifc::Contains<3>::value == false, "Unexpected value presence" );
	static_assert( tl_ifc::Contains<5>::value == true, "Unexpected value presence" );
	static_assert( tl_ifc::Contains<7>::value == true, "Unexpected value presence" );
	static_assert( tl_ifc::Contains<8>::value == true, "Unexpected value presence" );
	static_assert( tl_ifc::FindIndex<3>::value == -1, "Unexpected value presence" );
	static_assert( tl_ifc::FindIndex<5>::value == 0, "Unexpected value presence" );
	static_assert( tl_ifc::FindIndex<7>::value == 1, "Unexpected value presence" );
	static_assert( tl_ifc::FindIndex<8>::value == 2, "Unexpected value presence" );
	static_assert( tl_ifc::Reverse<>::type::ByIndex<0>::value == 8, "Unexpected type" );
	static_assert( tl_ifc::Reverse<>::type::ByIndex<1>::value == 7, "Unexpected type" );
	static_assert( tl_ifc::Reverse<>::type::ByIndex<2>::value == 5, "Unexpected type" );
	static_assert( tl_ifc::Split<0>::former::kNumValues == 0, "Unexpected size" );
	static_assert( tl_ifc::Split<0>::latter::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( tl_ifc::Split<0>::latter::ByIndex<1>::value == 7, "Unexpected type" );
	static_assert( tl_ifc::Split<0>::latter::ByIndex<2>::value == 8, "Unexpected type" );
	static_assert( tl_ifc::Split<1>::former::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( tl_ifc::Split<1>::latter::ByIndex<0>::value == 7, "Unexpected type" );
	static_assert( tl_ifc::Split<1>::latter::ByIndex<1>::value == 8, "Unexpected type" );
	static_assert( tl_ifc::Split<2>::former::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( tl_ifc::Split<2>::former::ByIndex<1>::value == 7, "Unexpected type" );
	static_assert( tl_ifc::Split<2>::latter::ByIndex<0>::value == 8, "Unexpected type" );
	static_assert( tl_ifc::Split<3>::former::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( tl_ifc::Split<3>::former::ByIndex<1>::value == 7, "Unexpected type" );
	static_assert( tl_ifc::Split<3>::former::ByIndex<2>::value == 8, "Unexpected type" );
	static_assert( tl_ifc::Split<3>::latter::kNumValues == 0, "Unexpected size" );
}



TEST( ValueList, Add )
{
	using tl_ifc = ValueList<int, 5, 7, 8>;
	using tl_empty = ValueList<int>;

	// Append/prepend on empty should be identical
	static_assert( std::is_same<tl_empty::Append<7>::type, tl_empty::Prepend<7>::type>::value, "Unexpected type" );

	using tl_added_f = tl_empty::Append<7>::type;
	using tl_added_if = tl_added_f::Prepend<5>::type;
	using tl_added_ifc = tl_added_if::Append<8>::type;
	static_assert( std::is_same<tl_ifc, tl_added_ifc>::value, "Unexpected type" );
}

///////////////////////////////////////////////////////////////////////////////
}
