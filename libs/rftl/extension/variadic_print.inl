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

inline span<char> var_snprintf_details( size_t& numNonNullCharsDesired, span<char> buffer, char const* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	span<char> const retVal = var_vsnprintf_details( numNonNullCharsDesired, buffer, fmt, args );
	va_end( args );
	return retVal;
}



inline span<char> var_vsnprintf( span<char> buffer, char const* fmt, va_list args )
{
	size_t unused = 0;
	return var_vsnprintf_details( unused, buffer, fmt, args );
}

inline span<char> var_vsnprintf_details( size_t& numNonNullCharsDesired, span<char> buffer, char const* fmt, va_list args )
{
	numNonNullCharsDesired = 0;

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
	numNonNullCharsDesired = static_cast<size_t>( signedNonNullCharsDesired );

	if( numNonNullCharsDesired >= buffer.size() )
	{
		// Truncation case
		// NOTE: Don't trust vsnprintf, make sure it's null-terminated
		buffer.back() = '\0';
		return buffer;
	}
	size_t const numNonNullCharsWritten = numNonNullCharsDesired;

	// NOTE: Don't trust vsnprintf, make sure it's null-terminated
	buffer[numNonNullCharsWritten] = '\0';
	size_t const charsWritten = numNonNullCharsWritten + sizeof( '\0' );
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
