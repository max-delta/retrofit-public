#include "stdafx.h"

#include "core_reflect/VariableTraits.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Not anonymous, so as to allow external linkage
namespace details{

// Namespace-level variables
float namespace_external_f;
float const namespace_external_cf = 0.f;
static double namespace_internal_d;
static double const namespace_internal_cd;
thread_local int16_t namespace_thread_i;
thread_local int16_t const namespace_thread_ci = 0;
constexpr uint8_t namespace_constexpr_u = 0;

// Class-level members
class MyClass
{
public:
	float instance_f;
	float const instance_cf;
	static double static_d;
	static double const static_cd;
	static thread_local int16_t thread_i;
	static thread_local int16_t const thread_ci;
	static constexpr uint8_t constexpr_u = 0;
};

}



TEST( Reflect, NamespaceVariables )
{
	struct test_external_f
	{
		using VT = VariableTraits<decltype( &details::namespace_external_f )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == false, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::namespace_external_f )>::value, "Unexpected type" );
	};

	struct test_external_cf
	{
		using VT = VariableTraits<decltype( &details::namespace_external_cf )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == true, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::namespace_external_cf )>::value, "Unexpected type" );
	};

	struct test_internal_d
	{
		using VT = VariableTraits<decltype( &details::namespace_internal_d )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == false, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::namespace_internal_d )>::value, "Unexpected type" );
	};

	struct test_internal_cd
	{
		using VT = VariableTraits<decltype( &details::namespace_internal_cd )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == true, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::namespace_internal_cd )>::value, "Unexpected type" );
	};

	struct test_thread_i
	{
		using VT = VariableTraits<decltype( &details::namespace_thread_i )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == false, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::namespace_thread_i )>::value, "Unexpected type" );
	};

	struct test_thread_ci
	{
		using VT = VariableTraits<decltype( &details::namespace_thread_ci )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == true, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::namespace_thread_ci )>::value, "Unexpected type" );
	};

	struct test_constexpr_u
	{
		using VT = VariableTraits<decltype( &details::namespace_constexpr_u )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == true, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::namespace_constexpr_u )>::value, "Unexpected type" );
	};
}



TEST( Reflect, StaticMemberVariables )
{
	struct test_static_d
	{
		using VT = VariableTraits<decltype( &details::MyClass::static_d )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == false, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::MyClass::static_d )>::value, "Unexpected type" );
	};

	struct test_static_cd
	{
		using VT = VariableTraits<decltype( &details::MyClass::static_cd )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == true, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::MyClass::static_cd )>::value, "Unexpected type" );
	};

	struct test_thread_i
	{
		using VT = VariableTraits<decltype( &details::MyClass::thread_i )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == false, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::MyClass::thread_i )>::value, "Unexpected type" );
	};

	struct test_thread_ci
	{
		using VT = VariableTraits<decltype( &details::MyClass::thread_ci )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == true, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::MyClass::thread_ci )>::value, "Unexpected type" );
	};

	struct test_constexpr_u
	{
		using VT = VariableTraits<decltype( &details::MyClass::constexpr_u )>;
		static_assert( VT::kVariableType == VariableType::FreeStanding, "Unexpected value" );
		static_assert( VT::kIsConst == true, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::MyClass::constexpr_u )>::value, "Unexpected type" );
	};
}



TEST( Reflect, NonStaticMemberVariables )
{
	struct test_instance_f
	{
		using VT = VariableTraits<decltype( &details::MyClass::instance_f )>;
		static_assert( VT::kVariableType == VariableType::Member, "Unexpected value" );
		static_assert( VT::kIsConst == false, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::MyClass::instance_f )>::value, "Unexpected type" );
	};

	struct test_instance_cf
	{
		using VT = VariableTraits<decltype( &details::MyClass::instance_cf )>;
		static_assert( VT::kVariableType == VariableType::Member, "Unexpected value" );
		static_assert( VT::kIsConst == true, "Unexpected value" );
		static_assert( std::is_same< VT::VariableType, decltype( details::MyClass::instance_cf )>::value, "Unexpected type" );
	};
}

///////////////////////////////////////////////////////////////////////////////
}}
