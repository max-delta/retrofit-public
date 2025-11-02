#pragma once

#include "rftl/charconv"
#include "rftl/string_view"
#include "rftl/span"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// HACK: At time of writing, MSVC doesn't have substr(...) as properly
//  constexpr, due to the bounds check not being marked constexpr
template<typename CharT>
inline constexpr auto substr_noexcept( rftl::basic_string_view<CharT> src, size_t pos, size_t count = rftl::basic_string_view<CharT>::npos ) -> rftl::basic_string_view<CharT>
{
	// Start past the end?
	if( pos > src.size() )
	{
		// This would've thrown an exception
		return {};
	}

	// Clamp size
	size_t newSize = src.size() - pos;
	if( count < newSize )
	{
		newSize = count;
	}

	return rftl::basic_string_view<CharT>( src.data() + pos, newSize );
}



// Removes leading characters
template<typename CharT>
inline constexpr auto trim_prefix_chars( rftl::basic_string_view<CharT> src, rftl::basic_string_view<CharT> charSet ) -> rftl::basic_string_view<CharT>
{
	size_t const pos = src.find_first_not_of( charSet );
	if( pos == src.npos )
	{
		return src;
	}

	return substr_noexcept( src, pos );
}

template<typename CharT, size_t SizeT>
inline constexpr auto trim_prefix_chars( rftl::basic_string_view<CharT> src, const CharT ( &charSet )[SizeT] ) -> rftl::basic_string_view<CharT>
{
	static_assert( SizeT > 0 );
	return trim_prefix_chars<CharT>( src, rftl::basic_string_view<CharT>( charSet, SizeT ) );
}



// Removes trailing characters
template<typename CharT>
inline constexpr auto trim_suffix_chars( rftl::basic_string_view<CharT> src, rftl::basic_string_view<CharT> charSet ) -> rftl::basic_string_view<CharT>
{
	size_t const pos = src.find_last_not_of( charSet );
	if( pos == src.npos )
	{
		return src;
	}

	return substr_noexcept( src, 0, pos + 1 );
}

template<typename CharT, size_t SizeT>
inline constexpr auto trim_suffix_chars( rftl::basic_string_view<CharT> src, const CharT ( &charSet )[SizeT] ) -> rftl::basic_string_view<CharT>
{
	static_assert( SizeT > 0 );
	return trim_suffix_chars<CharT>( src, rftl::basic_string_view<CharT>( charSet, SizeT ) );
}



// Try to remove surrounds, like "...", (...), [...], etc
// NOTE: Returning the same string as the one given is a signal of failure
template<typename CharT>
inline constexpr auto try_trim_surrounds( rftl::basic_string_view<CharT> src, CharT prefix, CharT suffix ) -> rftl::basic_string_view<CharT>
{
	rftl::basic_string_view<CharT> const falseResult = src;

	if( src.empty() )
	{
		return falseResult;
	}

	if( src.front() != prefix )
	{
		return falseResult;
	}
	src.remove_prefix( 1 );

	if( src.empty() )
	{
		return falseResult;
	}

	if( src.back() != suffix )
	{
		return falseResult;
	}
	src.remove_suffix( 1 );

	return src;
}



// Useful for example finding a comment character and blowing it away along
//  with everything past it
template<typename CharT>
inline constexpr auto trim_after_find_first_inclusive( rftl::basic_string_view<CharT> src, CharT ch ) -> rftl::basic_string_view<CharT>
{
	size_t const pos = src.find_first_of( ch );
	if( pos == src.npos )
	{
		return src;
	}

	return substr_noexcept( src, 0, pos );
}



// Useful for incrementally splitting strings
template<typename CharT>
inline auto strtok_view( rftl::basic_string_view<CharT>& remaining, rftl::basic_string_view<CharT> delimCharSet ) -> rftl::basic_string_view<CharT>
{
	rftl::basic_string_view<CharT> const src = remaining;

	size_t const pos = src.find_first_of( delimCharSet );
	if( pos == src.npos )
	{
		// Empty remainder indicates we're done tokenizing
		remaining = {};
		return src;
	}

	rftl::basic_string_view<CharT> const retVal = substr_noexcept( src, 0, pos );
	remaining = substr_noexcept( src, pos + 1 );
	return retVal;
}

template<typename CharT>
inline auto strtok_view( rftl::basic_string_view<CharT>& remaining, CharT delim ) -> rftl::basic_string_view<CharT>
{
	return strtok_view( remaining, rftl::basic_string_view<CharT>( &delim, 1 ) );
}

template<typename CharT, size_t SizeT>
inline auto strtok_view( rftl::basic_string_view<CharT>& remaining, const CharT ( &delimCharSet )[SizeT] ) -> rftl::basic_string_view<CharT>
{
	static_assert( SizeT > 0 );
	return strtok_view( remaining, rftl::basic_string_view<CharT>( delimCharSet, SizeT ) );
}



template<typename IntegerT>
inline bool parse_int( IntegerT& dest, rftl::string_view const& src )
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
