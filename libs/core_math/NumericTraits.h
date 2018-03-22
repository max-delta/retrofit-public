#pragma once
#include "core\meta\MemberTest.h"

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

RF_DECLARE_HAS_PUBLIC_MEMBER_NAME_TEST( NumericEmpty );
RF_DECLARE_HAS_PUBLIC_MEMBER_NAME_TEST( NumericIdentity );

// Got a wierd math type that you can somehow contort into being constexpr?
//  Then you may need to overload this elsewhere, or provide the appropriate
//  traits. Also, let me know how you managed to make a new type that is
//  constexpr, I want to know!
template<typename T>
struct NumericTraits
{
	static constexpr T Empty()
	{
		return EmptyImpl<T>();
	}

	static constexpr T Identity()
	{
		return IdentityImpl<T>();
	}


private:
	template<typename TYPE, typename std::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( TYPE, NumericEmpty ) == false, int>::type = 0>
	static constexpr T EmptyImpl()
	{
		return T{};
	}

	template<typename TYPE, typename std::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( TYPE, NumericEmpty ), int>::type = 0>
	static constexpr T EmptyImpl()
	{
		return T::NumericEmpty();
	}

	template<typename TYPE, typename std::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( TYPE, NumericIdentity ) == false, int>::type = 0>
	static constexpr T IdentityImpl()
	{
		return static_cast<T>( 1 );
	}

	template<typename TYPE, typename std::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( TYPE, NumericIdentity ), int>::type = 0>
	static constexpr T IdentityImpl()
	{
		return T::NumericIdentity();
	}
};

///////////////////////////////////////////////////////////////////////////////
}}
