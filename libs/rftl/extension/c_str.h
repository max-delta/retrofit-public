#pragma once
#include "rftl/string"
#include "rftl/string_view"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT>
using c_str_pair = rftl::pair<rftl::basic_string<CharT>, CharT const*>;

template<typename CharT>
auto c_str( typename rftl::basic_string_view<CharT> const& view ) -> c_str_pair<CharT>
{
	c_str_pair<CharT> retVal = { rftl::basic_string<CharT>( view ), nullptr };
	retVal.second = retVal.first.c_str();
	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////

// TODO: Try to remove all usages of this where possible
#define RFTLE_CSTR( VIEW ) ::rftl::details::c_str( VIEW ).second

///////////////////////////////////////////////////////////////////////////////
}
