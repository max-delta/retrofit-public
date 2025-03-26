#pragma once

#include "rftl/extension/byte_view.h"
#include "rftl/string"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT = char>
CharT format_nibble( uint8_t nibble )
{
	if( nibble <= 0x9 )
	{
		return static_cast<CharT>( '0' + nibble );
	}
	if( nibble <= 0xf )
	{
		return static_cast<CharT>( 'A' + ( nibble - 0xa ) );
	}
	return '#';
}



template<typename CharT>
void format_byte( CharT& upper, CharT& lower, uint8_t byte )
{
	upper = format_nibble<CharT>( ( byte & 0xf0u ) >> 4 );
	lower = format_nibble<CharT>( byte & 0x0fu );
}



// NOTE: Max bytes specifies how many bytes to read from the view, not the size
//  of the outputted string, which will be atleast twice the size of the data
template<typename CharT = char>
basic_string<CharT> to_string( byte_view const& view, size_t maxBytes )
{
	size_t numBytes = view.size();
	bool truncated = false;
	if( maxBytes < numBytes )
	{
		truncated = true;
		numBytes = maxBytes;
	}

	size_t const numChars = numBytes * 2 + ( truncated ? 3 : 0 );

	basic_string<CharT> retVal;
	retVal.reserve( numChars );

	for( size_t i = 0; i < numBytes; i++ )
	{
		uint8_t const byte = view.at<uint8_t>( i );
		char ch1, ch2 = 0;
		format_byte( ch1, ch2, byte );
		retVal.push_back( ch1 );
		retVal.push_back( ch2 );
	}

	if( truncated )
	{
		retVal.push_back( '.' );
		retVal.push_back( '.' );
		retVal.push_back( '.' );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
