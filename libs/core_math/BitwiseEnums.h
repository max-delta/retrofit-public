#pragma once
#include "rftl/type_traits"


// NOTE: Enable operators in your scope via a 'using namespace' statement
namespace RF::math::enums::bitwise {
///////////////////////////////////////////////////////////////////////////////

// Integer promotion is probably the worst feature ever added to C that still
//  has managed to survive to present day

template<typename T, typename Unused = typename rftl::enable_if<rftl::is_enum<T>::value, int>::type>
T operator~( T const& val )
{
	return static_cast<T>( ~static_cast<rftl::underlying_type<T>::type>( val ) );
}



template<typename T, typename Unused = typename rftl::enable_if<rftl::is_enum<T>::value, int>::type>
T& operator&=( T& lhs, T const& rhs )
{
	return lhs = static_cast<T>( lhs & rhs );
}



template<typename T, typename Unused = typename rftl::enable_if<rftl::is_enum<T>::value, int>::type>
T& operator|=( T& lhs, T const& rhs )
{
	return lhs = static_cast<T>( lhs | rhs );
}

///////////////////////////////////////////////////////////////////////////////
}
