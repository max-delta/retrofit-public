#pragma once

#include "rftl/extension/byte_view.h"
#include "rftl/string"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

string to_string( byte_view const& view, size_t maxLen )
{
	size_t numBytes = view.size();
	bool truncated = false;
	if( maxLen < numBytes )
	{
		truncated = true;
		numBytes = maxLen;
	}

	size_t const numChars = numBytes * 2 + ( truncated ? 3 : 0 );

	string retVal;
	retVal.reserve( numChars );

	static constexpr auto toChar = []( uint8_t nibble ) -> char
	{
		if( nibble <= 0x9 )
		{
			return static_cast<char>( '0' + nibble );
		}
		if( nibble <= 0xf )
		{
			return static_cast<char>( 'A' + ( nibble - 0xa ) );
		}
		return '#';
	};

	for( size_t i = 0; i < numBytes; i++ )
	{
		uint8_t const byte = view.at<uint8_t>( i );
		char const ch1 = toChar( ( byte & 0xf0u ) >> 4u );
		char const ch2 = toChar( byte & 0x0fu );
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
