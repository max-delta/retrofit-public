#include "stdafx.h"

#include "core_reflect/FunctionTraits.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

namespace{

// Free-standing functions
void void_call_void( void )
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
	static void void_call_void( void )
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
}



TEST( Reflect, FreeFunctions )
{
	using ft_void_call_void = FunctionTraits<decltype(void_call_void)>;
	static_assert( ft_void_call_void::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_void_call_void::FunctionType, decltype( void_call_void )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_void_call_void::FunctionPointerType, decltype( &void_call_void )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_void_call_void::ReturnType, void>::value, "Unexpected type" );
	static_assert( ft_void_call_void::ParamTypes::kNumTypes == 0, "Unexpected size" );

	using ft_f_call_f = FunctionTraits<decltype( f_call_f )>;
	static_assert( ft_f_call_f::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_f_call_f::FunctionType, decltype( f_call_f )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_f_call_f::FunctionPointerType, decltype( &f_call_f )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_f_call_f::ReturnType, float>::value, "Unexpected type" );
	static_assert( ft_f_call_f::ParamTypes::kNumTypes == 1, "Unexpected size" );
	static_assert( std::is_same< ft_f_call_f::ParamTypes::ByIndex<0>::Type, float>::value, "Unexpected type" );

	using ft_d_call_ifc = FunctionTraits<decltype( d_call_ifc )>;
	static_assert( ft_d_call_ifc::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_d_call_ifc::FunctionType, decltype( d_call_ifc )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::FunctionPointerType, decltype( &d_call_ifc )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ReturnType, double>::value, "Unexpected type" );
	static_assert( ft_d_call_ifc::ParamTypes::kNumTypes == 3, "Unexpected size" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<0>::Type, int>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<1>::Type, float>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<2>::Type, char>::value, "Unexpected type" );
}



TEST( Reflect, FreeFunctionPointers )
{
	// NOTE: Subtle differences when function pointer, as opposed to reference
	decltype( &void_call_void ) ptr_void_call_void = void_call_void;
	decltype( &f_call_f ) ptr_f_call_f = f_call_f;
	decltype( &d_call_ifc ) ptr_d_call_ifc = d_call_ifc;

	using ft_void_call_void = FunctionTraits<decltype( ptr_void_call_void )>;
	static_assert( ft_void_call_void::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_void_call_void::FunctionType, decltype( void_call_void )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_void_call_void::FunctionPointerType, decltype( ptr_void_call_void )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_void_call_void::ReturnType, void>::value, "Unexpected type" );
	static_assert( ft_void_call_void::ParamTypes::kNumTypes == 0, "Unexpected size" );

	using ft_f_call_f = FunctionTraits<decltype( ptr_f_call_f )>;
	static_assert( ft_f_call_f::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_f_call_f::FunctionType, decltype( f_call_f )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_f_call_f::FunctionPointerType, decltype( ptr_f_call_f )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_f_call_f::ReturnType, float>::value, "Unexpected type" );
	static_assert( ft_f_call_f::ParamTypes::kNumTypes == 1, "Unexpected size" );
	static_assert( std::is_same< ft_f_call_f::ParamTypes::ByIndex<0>::Type, float>::value, "Unexpected type" );

	using ft_d_call_ifc = FunctionTraits<decltype( ptr_d_call_ifc )>;
	static_assert( ft_d_call_ifc::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_d_call_ifc::FunctionType, decltype( d_call_ifc )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::FunctionPointerType, decltype( ptr_d_call_ifc )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ReturnType, double>::value, "Unexpected type" );
	static_assert( ft_d_call_ifc::ParamTypes::kNumTypes == 3, "Unexpected size" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<0>::Type, int>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<1>::Type, float>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<2>::Type, char>::value, "Unexpected type" );
}



TEST( Reflect, StaticFunctions )
{
	using ft_void_call_void = FunctionTraits<decltype( MyStaticClass::void_call_void )>;
	static_assert( ft_void_call_void::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_void_call_void::FunctionType, decltype( void_call_void )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_void_call_void::FunctionPointerType, decltype( &void_call_void )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_void_call_void::ReturnType, void>::value, "Unexpected type" );
	static_assert( ft_void_call_void::ParamTypes::kNumTypes == 0, "Unexpected size" );

	using ft_f_call_f = FunctionTraits<decltype( MyStaticClass::f_call_f )>;
	static_assert( ft_f_call_f::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_f_call_f::FunctionType, decltype( f_call_f )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_f_call_f::FunctionPointerType, decltype( &f_call_f )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_f_call_f::ReturnType, float>::value, "Unexpected type" );
	static_assert( ft_f_call_f::ParamTypes::kNumTypes == 1, "Unexpected size" );
	static_assert( std::is_same< ft_f_call_f::ParamTypes::ByIndex<0>::Type, float>::value, "Unexpected type" );

	using ft_d_call_ifc = FunctionTraits<decltype( MyStaticClass::d_call_ifc )>;
	static_assert( ft_d_call_ifc::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_d_call_ifc::FunctionType, decltype( d_call_ifc )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::FunctionPointerType, decltype( &d_call_ifc )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ReturnType, double>::value, "Unexpected type" );
	static_assert( ft_d_call_ifc::ParamTypes::kNumTypes == 3, "Unexpected size" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<0>::Type, int>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<1>::Type, float>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<2>::Type, char>::value, "Unexpected type" );
}



TEST( Reflect, StaticFunctionPointers )
{
	// NOTE: Subtle differences when function pointer, as opposed to reference
	decltype( &void_call_void ) ptr_void_call_void = MyStaticClass::void_call_void;
	decltype( &f_call_f ) ptr_f_call_f = MyStaticClass::f_call_f;
	decltype( &d_call_ifc ) ptr_d_call_ifc = MyStaticClass::d_call_ifc;

	using ft_void_call_void = FunctionTraits<decltype( ptr_void_call_void )>;
	static_assert( ft_void_call_void::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_void_call_void::FunctionType, decltype( void_call_void )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_void_call_void::FunctionPointerType, decltype( ptr_void_call_void )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_void_call_void::ReturnType, void>::value, "Unexpected type" );
	static_assert( ft_void_call_void::ParamTypes::kNumTypes == 0, "Unexpected size" );

	using ft_f_call_f = FunctionTraits<decltype( ptr_f_call_f )>;
	static_assert( ft_f_call_f::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_f_call_f::FunctionType, decltype( f_call_f )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_f_call_f::FunctionPointerType, decltype( ptr_f_call_f )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_f_call_f::ReturnType, float>::value, "Unexpected type" );
	static_assert( ft_f_call_f::ParamTypes::kNumTypes == 1, "Unexpected size" );
	static_assert( std::is_same< ft_f_call_f::ParamTypes::ByIndex<0>::Type, float>::value, "Unexpected type" );

	using ft_d_call_ifc = FunctionTraits<decltype( ptr_d_call_ifc )>;
	static_assert( ft_d_call_ifc::kCallType == CallType::FreeStanding, "Unexpected value" );
	static_assert( std::is_same< ft_d_call_ifc::FunctionType, decltype( d_call_ifc )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::FunctionPointerType, decltype( ptr_d_call_ifc )>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ReturnType, double>::value, "Unexpected type" );
	static_assert( ft_d_call_ifc::ParamTypes::kNumTypes == 3, "Unexpected size" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<0>::Type, int>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<1>::Type, float>::value, "Unexpected type" );
	static_assert( std::is_same< ft_d_call_ifc::ParamTypes::ByIndex<2>::Type, char>::value, "Unexpected type" );
}

///////////////////////////////////////////////////////////////////////////////
}}
