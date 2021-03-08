#include "stdafx.h"
#include "CharConvert.h"

#include "core/macros.h"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

size_t NumBytesExpectedInUtf8( char firstByte )
{
	if( ( firstByte & 0x80 ) == 0 )
	{
		// 0xxxxxxx
		return 1;
	}
	else if( ( firstByte & 0xE0 ) == 0xC0 )
	{
		// 110xxxxx 10xxxxxx
		return 2;
	}
	else if( ( firstByte & 0xF0 ) == 0xE0 )
	{
		// 1110xxxx 10xxxxxx 10xxxxxx
		return 3;
	}
	else if( ( firstByte & 0xF8 ) == 0xF0 )
	{
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		return 4;
	}
	else
	{
		// Invalid
		return 0;
	}
}



size_t NumPairsExpectedInUtf8( char16_t firstPair )
{
	if( firstPair >= 0xd800 && firstPair <= 0xdbff )
	{
		// First pair
		return 2;
	}
	else if( firstPair >= 0xdc00 && firstPair <= 0xdfff )
	{
		// Second pair, unexpected and invalid
		return 0;
	}
	else
	{
		// Standalone
		return 1;
	}
}



char32_t ConvertSingleUtf8ToUtf32( char const* buffer, size_t numBytes )
{
	switch( numBytes )
	{
		case 1:
		{
			// 0xxxxxxx
			return static_cast<char32_t>(
				( ( buffer[0] & 0x7f ) << 0 ) );
		}
		case 2:
		{
			// 110xxxxx 10xxxxxx
			return static_cast<char32_t>(
				( ( buffer[0] & 0x1f ) << 6 ) |
				( ( buffer[1] & 0x3f ) << 0 ) );
		}
		case 3:
		{
			// 1110xxxx 10xxxxxx 10xxxxxx
			return static_cast<char32_t>(
				( ( buffer[0] & 0x0f ) << 12 ) |
				( ( buffer[1] & 0x3f ) << 6 ) |
				( ( buffer[2] & 0x3f ) << 0 ) );
		}
		case 4:
		{
			// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			return static_cast<char32_t>(
				( ( buffer[0] & 0x07 ) << 18 ) |
				( ( buffer[1] & 0x3f ) << 12 ) |
				( ( buffer[2] & 0x3f ) << 6 ) |
				( ( buffer[3] & 0x3f ) << 0 ) );
		}
		default:
		{
			// Invalid
			return 0;
		}
	}
}



char32_t ConvertSingleUtf16ToUtf32( char16_t const* buffer, size_t numPairs )
{
	switch( numPairs )
	{
		case 1:
		{
			// Standalone
			return buffer[0];
		}
		case 2:
		{
			// Pairs
			char16_t const highSurrogate = buffer[0];
			char16_t const lowSurrogate = buffer[1];
			RF_ASSERT( highSurrogate >= 0xd800 );
			RF_ASSERT( highSurrogate <= 0xdbff );
			RF_ASSERT( lowSurrogate >= 0xdc00 );
			RF_ASSERT( lowSurrogate <= 0xdfff );
			char16_t const high10bit = highSurrogate - 0xd800u;
			char16_t const low10bit = lowSurrogate - 0xdc00u;
			RF_ASSERT( high10bit <= 0x3ff );
			RF_ASSERT( low10bit <= 0x3ff );
			char32_t const transformedTo20bit = static_cast<char32_t>( ( high10bit << 10 ) | low10bit );
			RF_ASSERT( transformedTo20bit <= 0xfffff );
			char32_t const codePoint = transformedTo20bit + 0x10000;
			return codePoint;
		}
		default:
		{
			// Invalid
			return 0;
		}
	}
}



size_t ConvertSingleUtf32ToUtf8( char32_t codePoint, char ( &destination )[4] )
{
	if( codePoint <= 0x7f )
	{
		// 0xxxxxxx
		destination[0] = static_cast<char>( codePoint );
		destination[1] = 0;
		destination[2] = 0;
		destination[3] = 0;
		return 1;
	}
	else if( codePoint <= 0x07ff )
	{
		// 110xxxxx 10xxxxxx
		destination[0] = static_cast<char>( 0xc0 | ( codePoint >> 6 ) );
		destination[1] = static_cast<char>( 0x80 | ( ( codePoint >> 0 ) & 0x3f ) );
		destination[2] = 0;
		destination[3] = 0;
		return 2;
	}
	else if( codePoint <= 0xffff )
	{
		// 1110xxxx 10xxxxxx 10xxxxxx
		destination[0] = static_cast<char>( 0xe0 | ( codePoint >> 12 ) );
		destination[1] = static_cast<char>( 0x80 | ( ( codePoint >> 6 ) & 0x3f ) );
		destination[2] = static_cast<char>( 0x80 | ( ( codePoint >> 0 ) & 0x3f ) );
		destination[3] = 0;
		return 3;
	}
	else if( codePoint <= 0x10ffff )
	{
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		destination[0] = static_cast<char>( 0xf0 | ( codePoint >> 18 ) );
		destination[1] = static_cast<char>( 0x80 | ( ( codePoint >> 12 ) & 0x3f ) );
		destination[2] = static_cast<char>( 0x80 | ( ( codePoint >> 6 ) & 0x3f ) );
		destination[3] = static_cast<char>( 0x80 | ( ( codePoint >> 0 ) & 0x3f ) );
		return 4;
	}
	else
	{
		// Invalid
		destination[0] = 0;
		destination[1] = 0;
		destination[2] = 0;
		destination[3] = 0;
		return 0;
	}
}



size_t ConvertSingleUtf32ToUtf16( char32_t codePoint, char16_t ( &destination )[2] )
{
	if( codePoint <= 0xd7ff )
	{
		// Basic multilingual plane
		destination[0] = static_cast<char16_t>( codePoint );
		destination[1] = 0;
		return 1;
	}
	else if( codePoint >= 0xe000 && codePoint <= 0xffff )
	{
		// Basic multilingual plane
		destination[0] = static_cast<char16_t>( codePoint );
		destination[1] = 0;
		return 1;
	}
	else if( codePoint >= 0x010000 && codePoint <= 0x10ffff )
	{
		// Supplementary plane
		char32_t const transformedTo20bit = codePoint - 0x10000;
		RF_ASSERT( transformedTo20bit <= 0xfffff );
		char16_t const high10bit = static_cast<char16_t>( transformedTo20bit >> 10 );
		RF_ASSERT( high10bit <= 0x3ff );
		char16_t const low10bit = transformedTo20bit & 0x3ff;
		RF_ASSERT( low10bit <= 0x3ff );
		char16_t const highSurrogate = high10bit + 0xd800u;
		RF_ASSERT( highSurrogate >= 0xd800 );
		RF_ASSERT( highSurrogate <= 0xdbff );
		char16_t const lowSurrogate = low10bit + 0xdc00u;
		RF_ASSERT( lowSurrogate >= 0xdc00 );
		RF_ASSERT( lowSurrogate <= 0xdfff );
		destination[0] = highSurrogate;
		destination[1] = lowSurrogate;
		return 2;
	}
	else if( codePoint >= 0xd800 && codePoint <= 0xdfff )
	{
		// Explicitly invalid
		destination[0] = 0;
		destination[1] = 0;
		return 0;
	}
	else
	{
		// Invalid
		destination[0] = 0;
		destination[1] = 0;
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
}
