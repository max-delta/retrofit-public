#pragma once
#include "variadic_print.h"

#include "rftl/extension/bounded_overwrite_iterator.h"
#include "rftl/cstdio"
#include "rftl/limits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

inline span<char> var_snprintf( span<char> buffer, char const* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	span<char> const retVal = var_vsnprintf( buffer, fmt, args );
	va_end( args );
	return retVal;
}



inline span<char> var_vsnprintf( span<char> buffer, char const* fmt, va_list args )
{
	if( buffer.size() <= 0 )
	{
		// Dumb input?
		return {};
	}

	int const signedNonNullCharsDesired = rftl::vsnprintf( buffer.data(), buffer.size(), fmt, args );
	if( signedNonNullCharsDesired <= 0 )
	{
		// Error case?
		buffer.front() = '\0';
		return buffer.subspan( 0, 1 );
	}
	static_assert( numeric_limits<unsigned int>::max() <= numeric_limits<size_t>::max() );
	size_t const nonNullCharsDesired = static_cast<size_t>( signedNonNullCharsDesired );

	if( nonNullCharsDesired >= buffer.size() )
	{
		// Truncation case
		// NOTE: Don't trust vsnprintf, make sure it's null-terminated
		buffer.back() = '\0';
		return buffer;
	}
	size_t const nonNullCharsWritten = nonNullCharsDesired;

	// NOTE: Don't trust vsnprintf, make sure it's null-terminated
	buffer[nonNullCharsWritten] = '\0';
	size_t const charsWritten = nonNullCharsWritten + sizeof( '\0' );
	return buffer.subspan( 0, charsWritten );
}



template<typename... ArgsT>
inline span<char> var_format_to_n( span<char> buffer, format_string<ArgsT...> fmt, ArgsT&&... args )
{
	if( buffer.empty() )
	{
		// Dumb input?
		return {};
	}

	bounded_forward_overwrite_iterator out( buffer );
	using ObnoxiousSize = decltype( out )::difference_type;
	ObnoxiousSize const size = static_cast<ObnoxiousSize>( buffer.size() );
	out = format_to_n( out, size, fmt, rftl::move( args... ) ).out;
	*out = '\0';
	*buffer.rbegin() = '\0';

	return span<char>( buffer.begin(), out.tell() );
}



inline span<char> var_vformat_to( span<char> buffer, string_view fmt, format_args args )
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

	return span<char>( buffer.begin(), out.tell() );
}

///////////////////////////////////////////////////////////////////////////////
}
