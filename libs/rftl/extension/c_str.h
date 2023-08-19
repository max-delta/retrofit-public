#pragma once
#include "rftl/string"
#include "rftl/string_view"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT>
auto c_str( rftl::basic_string_view<CharT> const& view ) -> typename CharT const*
{
	return rftl::basic_string<CharT>( view ).c_str();
}

///////////////////////////////////////////////////////////////////////////////
}
