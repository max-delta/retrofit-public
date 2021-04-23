#pragma once
#include "rftl/type_traits"


// NOTE: Enable operators in your scope via a 'using namespace' statement
namespace RF::enable_bitwise_enums {
///////////////////////////////////////////////////////////////////////////////

// Integer promotion is probably the worst feature ever added to C that still
//  has managed to survive to present day

template<typename T, typename Unused = typename rftl::enable_if<rftl::is_enum<T>::value, int>::type>
static constexpr T operator~( T const& val )
{
	return static_cast<T>(
		~static_cast<typename rftl::underlying_type<T>::type>( val ) );
}



template<typename T, typename Unused = typename rftl::enable_if<rftl::is_enum<T>::value, int>::type>
static constexpr T operator&( T const& lhs, T const& rhs )
{
	return static_cast<T>(
		static_cast<typename rftl::underlying_type<T>::type>( lhs ) &
		static_cast<typename rftl::underlying_type<T>::type>( rhs ) );
}



template<typename T, typename Unused = typename rftl::enable_if<rftl::is_enum<T>::value, int>::type>
static constexpr T operator|( T const& lhs, T const& rhs )
{
	return static_cast<T>(
		static_cast<typename rftl::underlying_type<T>::type>( lhs ) |
		static_cast<typename rftl::underlying_type<T>::type>( rhs ) );
}



template<typename T, typename Unused = typename rftl::enable_if<rftl::is_enum<T>::value, int>::type>
static constexpr T& operator&=( T& lhs, T const& rhs )
{
	return lhs = lhs & rhs;
}



template<typename T, typename Unused = typename rftl::enable_if<rftl::is_enum<T>::value, int>::type>
static constexpr T& operator|=( T& lhs, T const& rhs )
{
	return lhs = lhs | rhs;
}

///////////////////////////////////////////////////////////////////////////////
}
