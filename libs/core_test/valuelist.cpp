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
}

///////////////////////////////////////////////////////////////////////////////
}
