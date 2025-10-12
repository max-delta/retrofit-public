#pragma once
#include "cstring_view.h"

#include "rftl/cstdlib"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT>
inline basic_cstring_view<CharT>::basic_cstring_view( rftl::string const& str )
	: mPtr( str.c_str() )
{
	//
}



template<typename CharT>
inline basic_cstring_view<CharT>::basic_cstring_view( value_type const* ptr )
	: mPtr( ptr )
{
	//
}



template<typename CharT>
inline basic_cstring_view<CharT>::operator basic_string_view<typename basic_cstring_view<CharT>::value_type>() const
{
	if( mPtr == nullptr )
	{
		rftl::abort();
	}

	return basic_string_view<value_type>( mPtr );
}



template<typename CharT>
inline typename basic_cstring_view<CharT>::value_type const* basic_cstring_view<CharT>::c_str() const
{
	return mPtr;
}



template<typename CharT>
inline bool basic_cstring_view<CharT>::operator==(nullptr_t) const
{
	return mPtr == nullptr;
}

template<typename CharT>
inline bool basic_cstring_view<CharT>::operator!=( nullptr_t ) const
{
	return mPtr != nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}
