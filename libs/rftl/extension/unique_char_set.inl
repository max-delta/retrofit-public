#pragma once
#include "unique_char_set.h"

#include "rftl/algorithm"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT>
inline basic_unique_char_set<CharT>::basic_unique_char_set( initializer_list<typename Base::value_type> str )
	: basic_unique_char_set( view_type( str.begin(), str.end() ) )
{
	//
}



template<typename CharT>
inline basic_unique_char_set<CharT>::basic_unique_char_set( view_type str )
	: Base( str )
{
	Base& asStr = *this;
	sort( asStr.begin(), asStr.end() );
	typename Base::const_iterator last = unique( asStr.begin(), asStr.end() );
	asStr.erase( last, asStr.end() );
}



template<typename CharT>
inline size_t basic_unique_char_set<CharT>::count( typename Base::value_type ch ) const
{
	Base const& asStr = *this;
	return asStr.find( ch ) == Base::npos ? 0 : 1;
}

///////////////////////////////////////////////////////////////////////////////
}
