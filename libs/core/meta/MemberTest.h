#pragma once

#include "rftl/type_traits"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

// The test structure must be declared before use
#define RF_DECLARE_HAS_PUBLIC_MEMBER_NAME_TEST(NAME) \
	template<typename ___TYPE> \
	struct ___HasMemberWithName_##NAME \
	{ \
		/* A templatized signature that requires ::NAME to be valid during substitution */ \
		template<typename ___TYPETEST> static rftl::true_type ___ReturnValueSelector( decltype( ___TYPE::NAME )* ); \
		\
		/* A templatized signature that is extremely undesirable to the substitutor */ \
		template<typename ___TYPETEST> static rftl::false_type ___ReturnValueSelector( ... ); \
		\
		/* Extract the return value */ \
		using ___booltype = decltype( ___ReturnValueSelector<___TYPE>(nullptr) ); \
		\
		/* Extract the value */ \
		/* NOTE: Don't need the is_same<>, compiles correctly without it, but VS2017 */ \
		/*  Intellisense complains without it */ \
		static constexpr bool value = rftl::is_same<rftl::true_type, ___booltype>::value; \
	};

// The test is used in the form RF_HAS_PUBLIC_MEMBER_NAME( Class, DesiredMemberName )
#define RF_HAS_PUBLIC_MEMBER_NAME(CLASS, NAME) (___HasMemberWithName_##NAME<CLASS>::value)

// Assumes code similar to as follows:
//  template<class CLASS, typename rftl::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( CLASS, NAME ), int>::type = 0>
//  void foo();
//  template<class CLASS, typename rftl::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( CLASS, NAME ) == false, int>::type = 0>
//  void foo();

///////////////////////////////////////////////////////////////////////////////
}
