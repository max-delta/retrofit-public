#pragma once
#include "variadic_print.h"

#include "rftl/extension/bounded_overwrite_iterator.h"
#include "rftl/cstdio"
#include "rftl/limits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

inline string_view var_vsnprintf( char* buffer, size_t bufsz, char const* fmt, va_list args )
{
	if( bufsz <= 0 )
	{
		// Dumb input?
		return {};
	}

	string_view view = { buffer, bufsz };
	char& bufferBack = buffer[bufsz - 1];

	int const signedNonNullCharsDesired = rftl::vsnprintf( buffer, bufsz, fmt, args );
	if( signedNonNullCharsDesired <= 0 )
	{
		// Error case?
		buffer[0] = '\0';
		return view.substr( 0, 1 );
	}
	static_assert( numeric_limits<unsigned int>::max() <= numeric_limits<size_t>::max() );
	size_t const nonNullCharsDesired = static_cast<size_t>( signedNonNullCharsDesired );

	if( nonNullCharsDesired >= view.size() )
	{
		// Truncation case
		// NOTE: Don't trust vsnprintf, make sure it's null-terminated
		bufferBack = '\0';
		return view;
	}
	size_t const nonNullCharsWritten = nonNullCharsDesired;

	// NOTE: Don't trust vsnprintf, make sure it's null-terminated
	buffer[nonNullCharsWritten] = '\0';
	size_t const charsWritten = nonNullCharsWritten + sizeof( '\0' );
	return view.substr( 0, charsWritten );
}

inline string_view var_vsnprintf( span<char> buffer, char const* fmt, va_list args )
{
	return var_vsnprintf( buffer.data(), buffer.size(), fmt, args );
}



template<typename... ArgsT>
inline string_view var_format_to_n( char* buffer, size_t bufsz, format_string<ArgsT...> fmt, ArgsT&&... args )
{
	return var_format_to( span<char>( buffer, bufsz ), fmt, args... );
}

template<typename... ArgsT>
inline string_view var_format_to_n( span<char> buffer, format_string<ArgsT...> fmt, ArgsT&&... args )
{
	if( buffer.empty() )
	{
		// Dumb input?
		return {};
	}

	bounded_forward_overwrite_iterator out( buffer );
	using ObnoxiousSize = decltype(out)::difference_type;
	ObnoxiousSize const size = static_cast<ObnoxiousSize>( buffer.size() );
	out = format_to_n( out, size, fmt, rftl::move(args...) ).out;
	*out = '\0';
	*buffer.rbegin() = '\0';

	return string_view( buffer.begin(), out.tell() );
}



inline string_view var_vformat_to( char* buffer, size_t bufsz, string_view fmt, format_args args )
{
	return var_vformat_to( span<char>( buffer, bufsz ), fmt, args );
}

inline string_view var_vformat_to( span<char> buffer, string_view fmt, format_args args )
{
	if( buffer.empty() )
	{
		// Dumb input?
		return {};
	}

	bounded_forward_overwrite_iterator out( buffer );
	out = vformat_to( out, fmt, args );
	*out = '\0';
	*buffer.rbegin() = '\0';

	return string_view( buffer.begin(), out.tell() );
}

///////////////////////////////////////////////////////////////////////////////
}
