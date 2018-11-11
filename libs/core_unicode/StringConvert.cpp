#include "stdafx.h"
#include "StringConvert.h"
#include "CharConvert.h"

#include "core/macros.h"

#include "rftl/iterator"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT>
rftl::string CollapseToAscii( CharT const* source, size_t numElements )
{
	rftl::string retVal;
	retVal.reserve( numElements );
	for( size_t i = 0; i < numElements; i++ )
	{
		CharT const& in = source[i];

		if( in >= 0 && in <= 127 )
		{
			retVal.push_back( static_cast<char>( in ) );
		}
		else
		{
			retVal.push_back( '?' );
		}
	}
	return retVal;
}



template<typename CharT>
rftl::string CollapseToAscii( rftl::basic_string<CharT> const& source )
{
	return CollapseToAscii( source.data(), source.size() );
}

}
///////////////////////////////////////////////////////////////////////////////

rftl::string unicode::ConvertToASCII( char const * source, size_t numBytes )
{
	return details::CollapseToAscii( source, numBytes );
}

rftl::string ConvertToASCII( char16_t const * source, size_t numPairs )
{
	return details::CollapseToAscii( source, numPairs );
}

rftl::string ConvertToASCII( char32_t const * source, size_t numCodePoints )
{
	return details::CollapseToAscii( source, numCodePoints );
}

rftl::string ConvertToUtf8( char const * source, size_t numBytes )
{
	// Same
	return rftl::string( source, numBytes );
}

rftl::string ConvertToUtf8( char16_t const * source, size_t numPairs )
{
	// Shrink
	// HACK: Transition through UTF-32
	return ConvertToUtf8( ConvertToUtf32( source, numPairs ) );
}

rftl::string ConvertToUtf8( char32_t const * source, size_t numCodePoints )
{
	// Shrink
	rftl::string retVal;
	retVal.reserve( numCodePoints * 4 );
	for( size_t i = 0; i < numCodePoints; i++ )
	{
		char32_t const& codePoint = source[i];

		char temp[4] = {};
		size_t const numBytes = ConvertSingleUtf32ToUtf8( codePoint, temp );
		switch( numBytes )
		{
			case 1:
				retVal.push_back( temp[0] );
				break;
			case 2:
				retVal.push_back( temp[0] );
				retVal.push_back( temp[1] );
				break;
			case 3:
				retVal.push_back( temp[0] );
				retVal.push_back( temp[1] );
				retVal.push_back( temp[2] );
				break;
			case 4:
				retVal.push_back( temp[0] );
				retVal.push_back( temp[1] );
				retVal.push_back( temp[2] );
				retVal.push_back( temp[3] );
				break;
			default:
				retVal.push_back( '?' );
				break;
		}
	}
	return retVal;
}

rftl::u16string ConvertToUtf16( char const * source, size_t numBytes )
{
	// Expand
	// HACK: Transition through UTF-32
	return ConvertToUtf16( ConvertToUtf32( source, numBytes ) );
}

rftl::u16string ConvertToUtf16( char16_t const * source, size_t numPairs )
{
	// Same
	return rftl::u16string( source, numPairs );
}

rftl::u16string ConvertToUtf16( char32_t const * source, size_t numCodePoints )
{
	// Shrink
	rftl::u16string retVal;
	retVal.reserve( numCodePoints * 2 );
	for( size_t i = 0; i < numCodePoints; i++ )
	{
		char32_t const& codePoint = source[i];

		char16_t temp[2] = {};
		size_t const numPairs = ConvertSingleUtf32ToUtf16( codePoint, temp );
		switch( numPairs )
		{
			case 1:
				retVal.push_back( temp[0] );
				break;
			case 2:
				retVal.push_back( temp[0] );
				retVal.push_back( temp[1] );
				break;
			default:
				retVal.push_back( u'?' );
				break;
		}
	}
	return retVal;
}

rftl::u32string ConvertToUtf32( char const * source, size_t numBytes )
{
	// Expand
	rftl::u32string retVal;
	retVal.reserve( numBytes );
	size_t i = 0;
	while( i < numBytes )
	{
		char const* const buffer = &( source[i] );
		size_t const remainingBytes = numBytes - i;
		size_t const neededBytes = NumBytesExpectedInUtf8( *buffer );

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

		char32_t const codePoint = ConvertSingleUtf8ToUtf32( buffer, neededBytes );
		retVal.push_back( codePoint );

		i += neededBytes;
	}

	return retVal;
}

rftl::u32string ConvertToUtf32( char16_t const * source, size_t numPairs )
{
	// Expand
	rftl::u32string retVal;
	retVal.reserve( numPairs );
	size_t i = 0;
	while( i < numPairs )
	{
		char16_t const* const buffer = &( source[i] );
		size_t const remainingPairs = numPairs - i;
		size_t const neededPairs = NumPairsExpectedInUtf8( *buffer );

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

		char32_t const codePoint = ConvertSingleUtf16ToUtf32( buffer, neededPairs );
		retVal.push_back( codePoint );

		i += neededPairs;
	}

	return retVal;
}

rftl::u32string ConvertToUtf32( char32_t const * source, size_t numCodePoints )
{
	// Same
	return rftl::u32string( source, numCodePoints );
}

///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToASCII( rftl::string const & source )
{
	return ConvertToASCII( source.data(), source.size() );
}



rftl::string ConvertToASCII( rftl::u16string const & source )
{
	return ConvertToASCII( source.data(), source.size() );
}



rftl::string ConvertToASCII( rftl::u32string const & source )
{
	return ConvertToASCII( source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////

rftl::string ConvertToUtf8( rftl::string const & source )
{
	return ConvertToUtf8( source.data(), source.size() );
}



rftl::string ConvertToUtf8( rftl::u16string const & source )
{
	return ConvertToUtf8( source.data(), source.size() );
}



rftl::string ConvertToUtf8( rftl::u32string const & source )
{
	return ConvertToUtf8( source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////

rftl::u16string ConvertToUtf16( rftl::string const & source )
{
	return ConvertToUtf16( source.data(), source.size() );
}



rftl::u16string ConvertToUtf16( rftl::u16string const & source )
{
	return ConvertToUtf16( source.data(), source.size() );
}



rftl::u16string ConvertToUtf16( rftl::u32string const & source )
{
	return ConvertToUtf16( source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////

rftl::u32string ConvertToUtf32( rftl::string const & source )
{
	return ConvertToUtf32( source.data(), source.size() );
}



rftl::u32string ConvertToUtf32( rftl::u16string const & source )
{
	return ConvertToUtf32( source.data(), source.size() );
}



rftl::u32string ConvertToUtf32( rftl::u32string const & source )
{
	return ConvertToUtf32( source.data(), source.size() );
}

///////////////////////////////////////////////////////////////////////////////
}}
