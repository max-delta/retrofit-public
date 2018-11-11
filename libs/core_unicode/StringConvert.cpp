#include "stdafx.h"
#include "StringConvert.h"
#include "CharConvert.h"

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
