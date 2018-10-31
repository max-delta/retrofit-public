#include "stdafx.h"
#include "StringConvert.h"

#include "core/macros.h"

#include "rftl/iterator"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT>
rftl::string CollapseToAscii( rftl::basic_string<CharT> const& source )
{
	rftl::string retVal;
	retVal.reserve( source.size() );
	for( CharT const in : source )
	{
		if( in <= 127 )
		{
			retVal.push_back( static_cast<char>( in ) );
		}
		else
		{
			retVal.push_back( static_cast<char>( in ) );
		}
	}
	return retVal;
}



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
			char32_t const codePoint = transformedTo20bit + 0x100000;
			return codePoint;
		}
		default:
		{
			// Invalid
			return 0;
		}
	}
}

}
///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToASCII( rftl::string const & source )
{
	return details::CollapseToAscii( source );
}



rftl::string ConvertToASCII( rftl::u16string const & source )
{
	return details::CollapseToAscii( source );
}



rftl::string ConvertToASCII( rftl::u32string const & source )
{
	return details::CollapseToAscii( source );
}

///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToUtf8( rftl::string const & source )
{
	// Same
	return source;
}



rftl::string ConvertToUtf8( rftl::u16string const & source )
{
	// Shrink
	// HACK: Transition through UTF-32
	return ConvertToUtf8( ConvertToUtf32( source ) );
}



rftl::string ConvertToUtf8( rftl::u32string const & source )
{
	// Shrink
	rftl::string retVal;
	retVal.reserve( source.size() * 4 );
	for( char32_t const codePoint : source )
	{
		if( codePoint <= 0x7f )
		{
			// 0xxxxxxx
			retVal.push_back( static_cast<char>( codePoint ) );
		}
		else if( codePoint <= 0x07ff )
		{
			// 110xxxxx 10xxxxxx
			retVal.push_back( static_cast<char>( 0xc0 | ( codePoint >> 6 ) ) );
			retVal.push_back( static_cast<char>( 0x80 | ( ( codePoint >> 0 ) & 0x3f ) ) );
		}
		else if( codePoint <= 0xffff )
		{
			// 1110xxxx 10xxxxxx 10xxxxxx
			retVal.push_back( static_cast<char>( 0xe0 | ( codePoint >> 12 ) ) );
			retVal.push_back( static_cast<char>( 0x80 | ( ( codePoint >> 6 ) & 0x3f ) ) );
			retVal.push_back( static_cast<char>( 0x80 | ( ( codePoint >> 0 ) & 0x3f ) ) );
		}
		else if( codePoint <= 0x10ffff )
		{
			// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			retVal.push_back( static_cast<char>( 0xf0 | ( codePoint >> 18 ) ) );
			retVal.push_back( static_cast<char>( 0x80 | ( ( codePoint >> 12 ) & 0x3f ) ) );
			retVal.push_back( static_cast<char>( 0x80 | ( ( codePoint >> 6 ) & 0x3f ) ) );
			retVal.push_back( static_cast<char>( 0x80 | ( ( codePoint >> 0 ) & 0x3f ) ) );
		}
		else
		{
			// Invalid
			retVal.push_back( '?' );
		}
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::u16string ConvertToUtf16( rftl::string const & source )
{
	// Expand
	// HACK: Transition through UTF-32
	return ConvertToUtf16( ConvertToUtf32( source ) );
}



rftl::u16string ConvertToUtf16( rftl::u16string const & source )
{
	// Same
	return source;
}



rftl::u16string ConvertToUtf16( rftl::u32string const & source )
{
	// Shrink
	rftl::u16string retVal;
	retVal.reserve( source.size() * 2 );
	for( char32_t const codePoint : source )
	{
		if( codePoint <= 0xd7ff )
		{
			// Basic multilingual plane
			retVal.push_back( static_cast<char16_t>( codePoint ) );
		}
		else if( codePoint >= 0xe000 && codePoint <= 0xffff )
		{
			// Basic multilingual plane
			retVal.push_back( static_cast<char16_t>( codePoint ) );
		}
		else if( codePoint >= 0x010000 && codePoint <= 0x10ffff )
		{
			// Supplementary plane
			char32_t const transformedTo20bit = codePoint - 0x100000;
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
			retVal.push_back( highSurrogate );
			retVal.push_back( lowSurrogate );
		}
		else if( codePoint >= 0xd800 && codePoint <= 0xdfff )
		{
			// Explicitly invalid
			retVal.push_back( u'?' );
		}
		else
		{
			// Invalid
			retVal.push_back( u'?' );
		}
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

rftl::u32string ConvertToUtf32( rftl::string const & source )
{
	// Expand
	rftl::u32string retVal;
	retVal.reserve( source.size() );
	rftl::string::const_iterator iter = source.begin();
	while( iter != source.end() )
	{
		char const* const buffer = &( *iter );
		size_t const remainingBytes = static_cast<size_t>( rftl::distance( iter, source.end() ) );
		size_t const neededBytes = details::NumBytesExpectedInUtf8( *buffer );

		if( remainingBytes < neededBytes )
		{
			// Invalid, not enough bytes
			retVal.push_back( U'?' );
			break;
		}

		if( neededBytes == 0 )
		{
			// Invalid, unable to decode
			retVal.push_back( U'?' );
			break;
		}

		char32_t const codePoint = details::ConvertSingleUtf8ToUtf32( buffer, neededBytes );
		retVal.push_back( codePoint );

		iter += static_cast<std::string::difference_type>( neededBytes );
	}

	return retVal;
}



rftl::u32string ConvertToUtf32( rftl::u16string const & source )
{
	// Expand
	rftl::u32string retVal;
	retVal.reserve( source.size() );
	rftl::u16string::const_iterator iter = source.begin();
	while( iter != source.end() )
	{
		char16_t const* const buffer = &( *iter );
		size_t const remainingPairs = static_cast<size_t>( rftl::distance( iter, source.end() ) );
		size_t const neededPairs = details::NumPairsExpectedInUtf8( *buffer );

		if( remainingPairs < neededPairs )
		{
			// Invalid, not enough pairs
			retVal.push_back( U'?' );
			break;
		}

		if( neededPairs == 0 )
		{
			// Invalid, unable to decode
			retVal.push_back( U'?' );
			break;
		}

		char32_t const codePoint = details::ConvertSingleUtf16ToUtf32( buffer, neededPairs );
		retVal.push_back( codePoint );

		iter += static_cast<std::string::difference_type>( neededPairs );
	}

	return retVal;
}



rftl::u32string ConvertToUtf32( rftl::u32string const & source )
{
	return source;
}

///////////////////////////////////////////////////////////////////////////////
}}
