#include "stdafx.h"

#include "core/meta/ValueList.h"

#include "core/compiler.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( ValueList, Empty )
{
	// Should not cause compilation errors
	using vl_empty = ValueList<int>;
	static_assert( vl_empty::kNumValues == 0, "Unexpected size" );
	static_assert( vl_empty::Contains<5>::value == false, "Unexpected value presence" );
	static_assert( vl_empty::FindIndex<5>::value == -1, "Unexpected value presence" );
	static_assert( vl_empty::Reverse<>::type::kNumValues == 0, "Unexpected size" );
	static_assert( vl_empty::Split<0>::former::kNumValues == 0, "Unexpected size" );
	static_assert( vl_empty::Split<0>::latter::kNumValues == 0, "Unexpected size" );
}



TEST( ValueList, UnorthodoxValueTypes )
{
	// MSVC has internal compiler errors for non-empty lists with odd types
	#ifdef RF_PLATFORM_MSVC
	{
		// While ill-advised, should not cause compilation errors
		struct Unused;
		using vl_empty = ValueList<Unused const *>;
		static_assert( vl_empty::kNumValues == 0, "Unexpected size" );
		#ifndef __INTELLISENSE__ // Intellisense falls over in MSVC2017
			static_assert( vl_empty::Contains<reinterpret_cast<Unused const*>( 5 )>::value == false, "Unexpected value presence" );
			static_assert( vl_empty::FindIndex<reinterpret_cast<Unused const*>( 5 )>::value == -1, "Unexpected value presence" );
			static_assert( vl_empty::Reverse<>::type::kNumValues == 0, "Unexpected size" );
			static_assert( vl_empty::Split<0>::former::kNumValues == 0, "Unexpected size" );
			static_assert( vl_empty::Split<0>::latter::kNumValues == 0, "Unexpected size" );
		#endif
	}
	#else
	{
		// While ill-advised, should not cause compilation errors
		struct Stub
		{
			//
		};
		static constexpr Stub unused1;
		static constexpr Stub unused2;
		using vl_stub = ValueList<Stub const *, &unused2>;
		static_assert( vl_stub::kNumValues == 1, "Unexpected size" );
		static_assert( vl_stub::ByIndex<0>::value == &unused2, "Unexpected value" );
		static_assert( vl_stub::Contains<&unused1>::value == false, "Unexpected value presence" );
		static_assert( vl_stub::Contains<&unused2>::value == true, "Unexpected value presence" );
		static_assert( vl_stub::FindIndex<&unused1>::value == -1, "Unexpected value presence" );
		static_assert( vl_stub::FindIndex<&unused2>::value == 0, "Unexpected value presence" );
		static_assert( vl_stub::Reverse<>::type::kNumValues == 1, "Unexpected size" );
		static_assert( vl_stub::Reverse<>::type::ByIndex<0>::value == &unused2, "Unexpected type" );
		static_assert( vl_stub::Split<0>::former::kNumValues == 0, "Unexpected size" );
		static_assert( vl_stub::Split<0>::latter::kNumValues == 1, "Unexpected size" );
		static_assert( vl_stub::Split<0>::latter::ByIndex<0>::value == &unused2, "Unexpected size" );
	}
	#endif
}



TEST( ValueList, Single )
{
	// Should not cause logic errors in non-list usage
	using vl_5 = ValueList<int, 5>;
	static_assert( vl_5::kNumValues == 1, "Unexpected size" );
	static_assert( vl_5::ByIndex<0>::value == 5, "Unexpected value" );
	static_assert( vl_5::Contains<3>::value == false, "Unexpected value presence" );
	static_assert( vl_5::Contains<5>::value == true, "Unexpected value presence" );
	static_assert( vl_5::FindIndex<3>::value == -1, "Unexpected value presence" );
	static_assert( vl_5::FindIndex<5>::value == 0, "Unexpected value presence" );
	static_assert( vl_5::Reverse<>::type::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( vl_5::Split<0>::former::kNumValues == 0, "Unexpected size" );
	static_assert( vl_5::Split<0>::latter::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( vl_5::Split<1>::former::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( vl_5::Split<1>::latter::kNumValues == 0, "Unexpected size" );
}



TEST( ValueList, Multiple )
{
	using vl_578 = ValueList<int, 5, 7, 8>;
	static_assert( vl_578::kNumValues == 3, "Unexpected size" );
	static_assert( vl_578::ByIndex<0>::value == 5, "Unexpected value" );
	static_assert( vl_578::ByIndex<1>::value == 7, "Unexpected value" );
	static_assert( vl_578::ByIndex<2>::value == 8, "Unexpected value" );
	static_assert( vl_578::Contains<3>::value == false, "Unexpected value presence" );
	static_assert( vl_578::Contains<5>::value == true, "Unexpected value presence" );
	static_assert( vl_578::Contains<7>::value == true, "Unexpected value presence" );
	static_assert( vl_578::Contains<8>::value == true, "Unexpected value presence" );
	static_assert( vl_578::FindIndex<3>::value == -1, "Unexpected value presence" );
	static_assert( vl_578::FindIndex<5>::value == 0, "Unexpected value presence" );
	static_assert( vl_578::FindIndex<7>::value == 1, "Unexpected value presence" );
	static_assert( vl_578::FindIndex<8>::value == 2, "Unexpected value presence" );
	static_assert( vl_578::Reverse<>::type::ByIndex<0>::value == 8, "Unexpected type" );
	static_assert( vl_578::Reverse<>::type::ByIndex<1>::value == 7, "Unexpected type" );
	static_assert( vl_578::Reverse<>::type::ByIndex<2>::value == 5, "Unexpected type" );
	static_assert( vl_578::Split<0>::former::kNumValues == 0, "Unexpected size" );
	static_assert( vl_578::Split<0>::latter::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( vl_578::Split<0>::latter::ByIndex<1>::value == 7, "Unexpected type" );
	static_assert( vl_578::Split<0>::latter::ByIndex<2>::value == 8, "Unexpected type" );
	static_assert( vl_578::Split<1>::former::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( vl_578::Split<1>::latter::ByIndex<0>::value == 7, "Unexpected type" );
	static_assert( vl_578::Split<1>::latter::ByIndex<1>::value == 8, "Unexpected type" );
	static_assert( vl_578::Split<2>::former::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( vl_578::Split<2>::former::ByIndex<1>::value == 7, "Unexpected type" );
	static_assert( vl_578::Split<2>::latter::ByIndex<0>::value == 8, "Unexpected type" );
	static_assert( vl_578::Split<3>::former::ByIndex<0>::value == 5, "Unexpected type" );
	static_assert( vl_578::Split<3>::former::ByIndex<1>::value == 7, "Unexpected type" );
	static_assert( vl_578::Split<3>::former::ByIndex<2>::value == 8, "Unexpected type" );
	static_assert( vl_578::Split<3>::latter::kNumValues == 0, "Unexpected size" );
}



TEST( ValueList, Add )
{
	using vl_578 = ValueList<int, 5, 7, 8>;
	using vl_empty = ValueList<int>;

	// Append/prepend on empty should be identical
	static_assert( rftl::is_same<vl_empty::Append<7>::type, vl_empty::Prepend<7>::type>::value, "Unexpected type" );

	using vl_added_7 = vl_empty::Append<7>::type;
	using vl_added_57 = vl_added_7::Prepend<5>::type;
	using vl_added_578 = vl_added_57::Append<8>::type;
	static_assert( rftl::is_same<vl_578, vl_added_578>::value, "Unexpected type" );
}

///////////////////////////////////////////////////////////////////////////////
}
