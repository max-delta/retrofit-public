#include "stdafx.h"

#include "core_reflect/FunctionTraits.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

namespace{

// Free-standing functions
void v_call_v( void )
{
	return;
}
float f_call_f( float )
{
	return 0.f;
}
double d_call_ifc( int, float, char )
{
	return 0.;
}

// Static class members
class MyStaticClass
{
public:
	static void v_call_v( void )
	{
		return;
	}
	static float f_call_f( float )
	{
		return 0.f;
	}
	static double d_call_ifc( int, float, char )
	{
		return 0.;
	}
};

// Non-static non-const class members
class MyNonConstClass
{
public:
	void v_call_v( void )
	{
		return;
	}
	float f_call_f( float )
	{
		return 0.f;
	}
	double d_call_ifc( int, float, char )
	{
		return 0.;
	}
};

// Non-static const class members
class MyConstClass
{
public:
	void v_call_v( void ) const
	{
		return;
	}
	float f_call_f( float ) const
	{
		return 0.f;
	}
	double d_call_ifc( int, float, char ) const
	{
		return 0.;
	}
};
}



TEST( Reflect, FreeFunctions )
{
	struct test_v_call_v
	{
		using FT = FunctionTraits<decltype( v_call_v )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( &v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, void>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 0, "Unexpected size" );
	};

	struct test_f_call_f
	{
		using FT = FunctionTraits<decltype( f_call_f )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( &f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, float>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 1, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, float>::value, "Unexpected type" );
	};

	struct test_d_call_ifc
	{
		using FT = FunctionTraits<decltype( d_call_ifc )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( &d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, double>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 3, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, int>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<1>::type, float>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<2>::type, char>::value, "Unexpected type" );
	};
}



TEST( Reflect, FreeFunctionPointers )
{
	// NOTE: Subtle differences when function pointer, as opposed to reference
	decltype( &v_call_v ) ptr_v_call_v = v_call_v;
	decltype( &f_call_f ) ptr_f_call_f = f_call_f;
	decltype( &d_call_ifc ) ptr_d_call_ifc = d_call_ifc;

	struct test_v_call_v
	{
		using FT = FunctionTraits<decltype( ptr_v_call_v )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, void>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 0, "Unexpected size" );
	};

	struct test_f_call_f
	{
		using TestFunctionType = decltype( f_call_f );
		using FT = FunctionTraits<decltype( ptr_f_call_f )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, float>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 1, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, float>::value, "Unexpected type" );
	};

	struct test_d_call_ifc
	{
		using FT = FunctionTraits<decltype( ptr_d_call_ifc )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, double>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 3, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, int>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<1>::type, float>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<2>::type, char>::value, "Unexpected type" );
	};
}



TEST( Reflect, StaticFunctions )
{
	using Class = MyStaticClass;

	struct test_v_call_v
	{
		using FT = FunctionTraits<decltype( Class::v_call_v )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( Class::v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( &Class::v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, void>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 0, "Unexpected size" );
	};

	struct test_f_call_f
	{
		using FT = FunctionTraits<decltype( Class::f_call_f )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( Class::f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( &Class::f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, float>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 1, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, float>::value, "Unexpected type" );
	};

	struct test_d_call_ifc
	{
		using FT = FunctionTraits<decltype( Class::d_call_ifc )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( Class::d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( &Class::d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, double>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 3, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, int>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<1>::type, float>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<2>::type, char>::value, "Unexpected type" );
	};
}



TEST( Reflect, StaticFunctionPointers )
{
	using Class = MyStaticClass;

	// NOTE: Subtle differences when function pointer, as opposed to reference
	decltype( &Class::v_call_v ) ptr_v_call_v = Class::v_call_v;
	decltype( &Class::f_call_f ) ptr_f_call_f = Class::f_call_f;
	decltype( &Class::d_call_ifc ) ptr_d_call_ifc = Class::d_call_ifc;

	struct test_v_call_v
	{
		using FT = FunctionTraits<decltype( ptr_v_call_v )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( Class::v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, void>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 0, "Unexpected size" );
	};

	struct test_f_call_f
	{
		using FT = FunctionTraits<decltype( ptr_f_call_f )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( Class::f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, float>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 1, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, float>::value, "Unexpected type" );
	};

	struct test_d_call_ifc
	{
		using FT = FunctionTraits<decltype( ptr_d_call_ifc )>;
		static_assert( FT::kCallType == CallType::FreeStanding, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionType, decltype( Class::d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, double>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 3, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, int>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<1>::type, float>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<2>::type, char>::value, "Unexpected type" );
	};
}



TEST( Reflect, NonConstFunctionPointers )
{
	using Class = MyNonConstClass;

	// NOTE: Classes don't allow non-pointer member referencing
	decltype( &Class::v_call_v ) ptr_v_call_v = &Class::v_call_v;
	decltype( &Class::f_call_f ) ptr_f_call_f = &Class::f_call_f;
	decltype( &Class::d_call_ifc ) ptr_d_call_ifc = &Class::d_call_ifc;

	struct test_v_call_v
	{
		using FT = FunctionTraits<decltype( ptr_v_call_v )>;
		static_assert( FT::kCallType == CallType::Member, "Unexpected value" );
		static_assert( FT::kRequiresConst == false, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, void>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 0, "Unexpected size" );
	};

	struct test_f_call_f
	{
		using FT = FunctionTraits<decltype( ptr_f_call_f )>;
		static_assert( FT::kCallType == CallType::Member, "Unexpected value" );
		static_assert( FT::kRequiresConst == false, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, float>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 1, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, float>::value, "Unexpected type" );
	};

	struct test_d_call_ifc
	{
		using FT = FunctionTraits<decltype( ptr_d_call_ifc )>;
		static_assert( FT::kCallType == CallType::Member, "Unexpected value" );
		static_assert( FT::kRequiresConst == false, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, double>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 3, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, int>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<1>::type, float>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<2>::type, char>::value, "Unexpected type" );
	};
}



TEST( Reflect, ConstFunctionPointers )
{
	using Class = MyConstClass;

	// NOTE: Classes don't allow non-pointer member referencing
	decltype( &Class::v_call_v ) ptr_v_call_v = &Class::v_call_v;
	decltype( &Class::f_call_f ) ptr_f_call_f = &Class::f_call_f;
	decltype( &Class::d_call_ifc ) ptr_d_call_ifc = &Class::d_call_ifc;

	struct test_v_call_v
	{
		using FT = FunctionTraits<decltype( ptr_v_call_v )>;
		static_assert( FT::kCallType == CallType::Member, "Unexpected value" );
		static_assert( FT::kRequiresConst, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_v_call_v )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, void>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 0, "Unexpected size" );
	};

	struct test_f_call_f
	{
		using FT = FunctionTraits<decltype( ptr_f_call_f )>;
		static_assert( FT::kCallType == CallType::Member, "Unexpected value" );
		static_assert( FT::kRequiresConst, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_f_call_f )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, float>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 1, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, float>::value, "Unexpected type" );
	};

	struct test_d_call_ifc
	{
		using FT = FunctionTraits<decltype( ptr_d_call_ifc )>;
		static_assert( FT::kCallType == CallType::Member, "Unexpected value" );
		static_assert( FT::kRequiresConst, "Unexpected value" );
		static_assert( rftl::is_same< FT::FunctionPointerType, decltype( ptr_d_call_ifc )>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ReturnType, double>::value, "Unexpected type" );
		static_assert( FT::ParamTypes::kNumTypes == 3, "Unexpected size" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<0>::type, int>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<1>::type, float>::value, "Unexpected type" );
		static_assert( rftl::is_same< FT::ParamTypes::ByIndex<2>::type, char>::value, "Unexpected type" );
	};
}

///////////////////////////////////////////////////////////////////////////////
}}
