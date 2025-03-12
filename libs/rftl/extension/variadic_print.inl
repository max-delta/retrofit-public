#pragma once
#include "variadic_print.h"

#include "rftl/cstdio"
#include "rftl/limits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

string_view var_snprintf( char* buffer, size_t bufsz, char const* fmt, va_list args )
{
	if( bufsz <= 0 )
	{
		// Dumb input?
		return {};
	}

	string_view view = { buffer, bufsz };
	char& bufferBack = buffer[bufsz - 1];

	int const signedNonNullCharsDesired = vsnprintf( buffer, bufsz, fmt, args );
	if( signedNonNullCharsDesired <= 0 )
	{
		// Error case?
		buffer[0] = '\0';
		return view.substr( 0, 1 );
	}
	static_assert( rftl::numeric_limits<unsigned int>::max() <= rftl::numeric_limits<size_t>::max() );
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

///////////////////////////////////////////////////////////////////////////////
}
