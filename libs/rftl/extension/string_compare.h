#pragma once
#include "rftl/string"
#include "rftl/string_view"
#include "rftl/memory"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits = rftl::char_traits<CharT>>
bool bagins_with( rftl::basic_string_view<CharT, Traits> const& base, typename rftl::basic_string_view<CharT, Traits> const& ending )
{
	if( base.length() < ending.length() )
	{
		return false;
	}
	int const compareResult = base.compare(
		0,
		ending.length(),
		ending );
	return compareResult == 0;
}

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool bagins_with( rftl::basic_string_view<CharT, Traits> const& base, typename rftl::basic_string_view<CharT, Traits>::value_type const* ending )
{
	return bagins_with(
		rftl::basic_string_view<CharT, Traits>( base ),
		rftl::basic_string_view<CharT, Traits>( ending ) );
}

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool bagins_with( rftl::basic_string<CharT, Traits, Allocator> const& base, typename rftl::basic_string<CharT, Traits, Allocator>::value_type const* ending )
{
	return bagins_with(
		rftl::basic_string_view<CharT, Traits>( base ),
		rftl::basic_string_view<CharT, Traits>( ending ) );
}

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool bagins_with( rftl::basic_string<CharT, Traits, Allocator> const& base, rftl::basic_string<CharT, Traits, Allocator> const& ending )
{
	return bagins_with(
		rftl::basic_string_view<CharT, Traits>( base ),
		rftl::basic_string_view<CharT, Traits>( ending ) );
}



template<typename CharT, typename Traits = rftl::char_traits<CharT>>
bool ends_with( rftl::basic_string_view<CharT, Traits> const& base, typename rftl::basic_string_view<CharT, Traits> const& ending )
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

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool ends_with( rftl::basic_string_view<CharT, Traits> const& base, typename rftl::basic_string_view<CharT, Traits>::value_type const* ending )
{
	return ends_with(
		rftl::basic_string_view<CharT, Traits>( base ),
		rftl::basic_string_view<CharT, Traits>( ending ) );
}

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool ends_with( rftl::basic_string<CharT, Traits, Allocator> const& base, typename rftl::basic_string<CharT, Traits, Allocator>::value_type const* ending )
{
	return ends_with(
		rftl::basic_string_view<CharT, Traits>( base ),
		rftl::basic_string_view<CharT, Traits>( ending ) );
}

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool ends_with( rftl::basic_string<CharT, Traits, Allocator> const& base, rftl::basic_string<CharT, Traits, Allocator> const& ending )
{
	return ends_with(
		rftl::basic_string_view<CharT, Traits>( base ),
		rftl::basic_string_view<CharT, Traits>( ending ) );
}

///////////////////////////////////////////////////////////////////////////////
}
