#pragma once
#include "rftl/type_traits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// By default, invalid
template<typename EnumT, typename UnusedT = EnumT>
struct conditional_underlying_unscoped_type
{
};

// Use underlying type of enums
template<typename EnumT>
struct conditional_underlying_unscoped_type<EnumT, typename rftl::enable_if<rftl::is_enum<EnumT>::value, EnumT>::type> : public rftl::underlying_type<EnumT>
{
	static_assert( __cplusplus < 202002L, "Make this check only work when is_scoped_enum is false" );
};

// Use type for non-enums
template<typename EnumT>
struct conditional_underlying_unscoped_type<EnumT, typename rftl::enable_if<rftl::is_enum<EnumT>::value == false, EnumT>::type>
{
	using type = EnumT;
};

///////////////////////////////////////////////////////////////////////////////
}
