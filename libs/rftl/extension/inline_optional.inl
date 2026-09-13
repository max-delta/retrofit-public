#pragma once
#include "inline_optional.h"

#include "core/rf_assert.h"

RF_CPP23_TODO( "Make asserts work using if-consteval" );
#include "core/meta/FailConsteval.h"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////
namespace details {

RF_CPP23_TODO( "Make asserts work using if-consteval" );
inline constexpr void inline_optional_constexpr_assert( bool val )
{
	if( rftl::is_constant_evaluated() )
	{
		if( val == false )
		{
			RF::FailConsteval( "Inline optional assert failed" );
		}
	}
	else
	{
		constexpr auto assertion = []( bool val ) -> void
		{
			RF_ASSERT( val );
		};
		assertion( val );
	}
}

}
///////////////////////////////////////////////////////////////////////////////

template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::inline_optional( nullopt_t )
	: inline_optional()
{
	//
}



template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::inline_optional( value_type&& value )
	: mVal( rftl::move( value ) )
{
	details::inline_optional_constexpr_assert( has_value() );
}



template<typename T, T InvalidVal>
template<typename... ArgsT>
inline constexpr inline_optional<T, InvalidVal>::inline_optional( in_place_t, ArgsT&&... args )
	: mVal( rftl::forward<ArgsT>( args )... )
{
	details::inline_optional_constexpr_assert( has_value() );
}



template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type const* inline_optional<T, InvalidVal>::operator->() const
{
	details::inline_optional_constexpr_assert( has_value() );
	return &mVal;
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type* inline_optional<T, InvalidVal>::operator->()
{
	details::inline_optional_constexpr_assert( has_value() );
	return &mVal;
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type const& inline_optional<T, InvalidVal>::operator*() const&
{
	return value();
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type& inline_optional<T, InvalidVal>::operator*() &
{
	return value();
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type const&& inline_optional<T, InvalidVal>::operator*() const&&
{
	return rftl::move( value() );
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type&& inline_optional<T, InvalidVal>::operator*() &&
{
	return rftl::move( value() );
}



template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::operator bool() const
{
	return has_value();
}
template<typename T, T InvalidVal>
inline constexpr bool inline_optional<T, InvalidVal>::has_value() const
{
	return mVal != kInvalid;
}



template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type const& inline_optional<T, InvalidVal>::value() const&
{
	details::inline_optional_constexpr_assert( has_value() );
	return mVal;
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type& inline_optional<T, InvalidVal>::value() &
{
	details::inline_optional_constexpr_assert( has_value() );
	return mVal;
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type const&& inline_optional<T, InvalidVal>::value() const&&
{
	details::inline_optional_constexpr_assert( has_value() );
	return rftl::move( mVal );
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type&& inline_optional<T, InvalidVal>::value() &&
{
	details::inline_optional_constexpr_assert( has_value() );
	return rftl::move( mVal );
}



template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type inline_optional<T, InvalidVal>::value_or( value_type&& fallback ) const&
{
	return has_value() ? value() : fallback;
}
template<typename T, T InvalidVal>
inline constexpr inline_optional<T, InvalidVal>::value_type inline_optional<T, InvalidVal>::value_or( value_type&& fallback ) &&
{
	return has_value() ? value() : fallback;
}

///////////////////////////////////////////////////////////////////////////////
}
