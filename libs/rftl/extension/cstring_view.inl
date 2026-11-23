#pragma once
#include "cstring_view.h"

#include "core/rf_assert.h"

#include "rftl/cstdlib"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT>
inline basic_cstring_view<CharT>::basic_cstring_view( rftl::string const& str )
	: mPtr( str.c_str() )
	, mSizeWithoutTerminator( basic_string_view<CharT>( mPtr ).size() )
{
	//
}



template<typename CharT>
inline basic_cstring_view<CharT>::basic_cstring_view( span_type str, assure_terminated_type )
	: mPtr( str.data() )
	, mSizeWithoutTerminator( str.size() )
{
	// Trust but verify
	RF_ASSERT( mPtr[mSizeWithoutTerminator] == CharT( '\0' ) );
}


template<typename CharT>
inline basic_cstring_view<CharT>::basic_cstring_view( span_type str, includes_terminator_type )
	: mPtr( str.data() )
	, mSizeWithoutTerminator( str.size() - 1 )
{
	// Trust but verify
	RF_ASSERT( mPtr[mSizeWithoutTerminator] == CharT( '\0' ) );
}



template<typename CharT>
inline basic_cstring_view<CharT>::basic_cstring_view( value_type const* ptr )
	: mPtr( ptr )
	, mSizeWithoutTerminator( mPtr == nullptr ? 0 : basic_string_view<CharT>( mPtr ).size() )
{
	//
}



template<typename CharT>
inline basic_cstring_view<CharT>::operator view_type() const
{
	if( mPtr == nullptr )
	{
		rftl::abort();
	}

	return force_view();
}



template<typename CharT>
inline basic_cstring_view<CharT>::operator span_type() const
{
	return force_span();
}



template<typename CharT>
inline basic_cstring_view<CharT>::view_type basic_cstring_view<CharT>::force_view() const
{
	return view_type( mPtr, mSizeWithoutTerminator );
}



template<typename CharT>
inline basic_cstring_view<CharT>::span_type basic_cstring_view<CharT>::force_span() const
{
	if( mPtr == nullptr )
	{
		return span_type( nullptr, 0 );
	}
	else
	{
		return span_type( mPtr, mSizeWithoutTerminator + 1 );
	}
}



template<typename CharT>
inline typename basic_cstring_view<CharT>::value_type const* basic_cstring_view<CharT>::c_str() const
{
	return mPtr;
}



template<typename CharT>
inline bool basic_cstring_view<CharT>::operator==( nullptr_t ) const
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
