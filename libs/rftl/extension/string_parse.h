#pragma once

#include "rftl/charconv"
#include "rftl/string_view"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename IntegerT>
bool parse_int( IntegerT& dest, rftl::string_view const& src )
{
	IntegerT temp = {};
	char const* const begin = src.data();
	char const* const end = begin + src.length();
	rftl::from_chars_result const result = rftl::from_chars( begin, end, temp, 10 );
	bool const success = result.ptr == end;
	dest = success ? temp : IntegerT{};
	return success;
}

///////////////////////////////////////////////////////////////////////////////
}
