#pragma once
#include "rftl/string"
#include "rftl/memory"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool ends_with( rftl::basic_string<CharT, Traits, Allocator> const& base, typename rftl::basic_string<CharT, Traits, Allocator>::value_type const* ending )
{
	rftl::basic_string<CharT, Traits, Allocator> const endingStr = ending;
	if( base.length() < endingStr.length() )
	{
		return false;
	}
	int const compareResult = base.compare(
		base.length() - endingStr.length(),
		endingStr.length(),
		endingStr );
	return compareResult == 0;
}


template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool ends_with( rftl::basic_string<CharT, Traits, Allocator> const& base, rftl::basic_string<CharT, Traits, Allocator> const& ending )
{
	if( base.length() < ending.length() )
	{
		return false;
	}
	int const compareResult = base.compare(
		base.length() - ending.length(),
		ending.length(),
		ending );
	return compareResult == 0;
}

///////////////////////////////////////////////////////////////////////////////
}
