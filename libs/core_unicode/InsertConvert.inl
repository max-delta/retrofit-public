#include "stdafx.h"
#include "InsertConvert.h"
#include "CharConvert.h"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT, typename InserterT>
void CollapseToAscii( CharT const* source, size_t numElements, InserterT insertIterator )
{
	for( size_t i = 0; i < numElements; i++ )
	{
		CharT const& in = source[i];

		if( in >= 0 && in <= 127 )
		{
			insertIterator = static_cast<char>( in );
		}
		else
		{
			insertIterator = '?';
		}
	}
}

}
///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertSingleUtf32ToUtf8( char32_t codePoint, InserterT insertIterator )
{
	char temp[4] = {};
	size_t const numBytes = ConvertSingleUtf32ToUtf8( codePoint, temp );
	switch( numBytes )
	{
		case 1:
			insertIterator = temp[0];
			break;
		case 2:
			insertIterator = temp[0];
			insertIterator = temp[1];
			break;
		case 3:
			insertIterator = temp[0];
			insertIterator = temp[1];
			insertIterator = temp[2];
			break;
		case 4:
			insertIterator = temp[0];
			insertIterator = temp[1];
			insertIterator = temp[2];
			insertIterator = temp[3];
			break;
		default:
			insertIterator = '?';
			break;
	}
}



template<typename InserterT> void ConvertSingleUtf32ToUtf16( char32_t codePoint, InserterT insertIterator )
{
	char16_t temp[2] = {};
	size_t const numPairs = ConvertSingleUtf32ToUtf16( codePoint, temp );
	switch( numPairs )
	{
		case 1:
			insertIterator = temp[0];
			break;
		case 2:
			insertIterator = temp[0];
			insertIterator = temp[1];
			break;
		default:
			insertIterator = u'?';
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToASCII( char const* source, size_t numBytes, InserterT insertIterator )
{
	details::CollapseToAscii( source, numBytes, insertIterator );
}



template<typename InserterT> void ConvertToASCII( char16_t const* source, size_t numPairs, InserterT insertIterator )
{
	details::CollapseToAscii( source, numPairs, insertIterator );
}



template<typename InserterT> void ConvertToASCII( char32_t const* source, size_t numCodePoints, InserterT insertIterator )
{
	details::CollapseToAscii( source, numCodePoints, insertIterator );
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf8( char const* source, size_t numBytes, InserterT insertIterator )
{
	// Same
	for( size_t i = 0; i < numBytes; i++ )
	{
		insertIterator = source[i];
	}
}



template<typename InserterT> void ConvertToUtf8( char16_t const* source, size_t numPairs, InserterT insertIterator )
{
	// Transition through UTF-32
	size_t i = 0;
	while( i < numPairs )
	{
		char16_t const* const buffer = &( source[i] );
		size_t const remainingPairs = numPairs - i;
		size_t const neededPairs = NumPairsExpectedInUtf8( *buffer );

		if( remainingPairs < neededPairs )
		{
			// Invalid, not enough pairs
			insertIterator = U'?';
			break;
		}

		if( neededPairs == 0 )
		{
			// Invalid, unable to decode
			insertIterator = U'?';
			break;
		}

		// Expand
		char32_t const codePoint = ConvertSingleUtf16ToUtf32( buffer, neededPairs );

		// Shrink
		ConvertSingleUtf32ToUtf8( codePoint, insertIterator );

		i += neededPairs;
	}
}



template<typename InserterT> void ConvertToUtf8( char32_t const* source, size_t numCodePoints, InserterT insertIterator )
{
	// Shrink
	for( size_t i = 0; i < numCodePoints; i++ )
	{
		char32_t const& codePoint = source[i];
		ConvertSingleUtf32ToUtf8( codePoint, insertIterator );
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf16( char const* source, size_t numBytes, InserterT insertIterator )
{
	// Transition through UTF-32
	size_t i = 0;
	while( i < numBytes )
	{
		char const* const buffer = &( source[i] );
		size_t const remainingBytes = numBytes - i;
		size_t const neededBytes = NumBytesExpectedInUtf8( *buffer );

		if( remainingBytes < neededBytes )
		{
			// Invalid, not enough bytes
			insertIterator = U'?';
			break;
		}

		if( neededBytes == 0 )
		{
			// Invalid, unable to decode
			insertIterator = U'?';
			break;
		}

		// Expand
		char32_t const codePoint = ConvertSingleUtf8ToUtf32( buffer, neededBytes );

		// Shrink
		ConvertSingleUtf32ToUtf16( codePoint, insertIterator );

		i += neededBytes;
	}
}



template<typename InserterT> void ConvertToUtf16( char16_t const* source, size_t numPairs, InserterT insertIterator )
{
	// Same
	for( size_t i = 0; i < numPairs; i++ )
	{
		insertIterator = source[i];
	}
}



template<typename InserterT> void ConvertToUtf16( char32_t const* source, size_t numCodePoints, InserterT insertIterator )
{
	// Shrink
	for( size_t i = 0; i < numCodePoints; i++ )
	{
		char32_t const& codePoint = source[i];
		ConvertSingleUtf32ToUtf16( codePoint, insertIterator );
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf32( char const* source, size_t numBytes, InserterT insertIterator )
{
	// Expand
	size_t i = 0;
	while( i < numBytes )
	{
		char const* const buffer = &( source[i] );
		size_t const remainingBytes = numBytes - i;
		size_t const neededBytes = NumBytesExpectedInUtf8( *buffer );

		if( remainingBytes < neededBytes )
		{
			// Invalid, not enough bytes
			insertIterator = U'?';
			break;
		}

		if( neededBytes == 0 )
		{
			// Invalid, unable to decode
			insertIterator = U'?';
			break;
		}

		char32_t const codePoint = ConvertSingleUtf8ToUtf32( buffer, neededBytes );
		insertIterator = codePoint;

		i += neededBytes;
	}
}



template<typename InserterT> void ConvertToUtf32( char16_t const* source, size_t numPairs, InserterT insertIterator )
{
	// Expand
	size_t i = 0;
	while( i < numPairs )
	{
		char16_t const* const buffer = &( source[i] );
		size_t const remainingPairs = numPairs - i;
		size_t const neededPairs = NumPairsExpectedInUtf8( *buffer );

		if( remainingPairs < neededPairs )
		{
			// Invalid, not enough pairs
			insertIterator = U'?';
			break;
		}

		if( neededPairs == 0 )
		{
			// Invalid, unable to decode
			insertIterator = U'?';
			break;
		}

		char32_t const codePoint = ConvertSingleUtf16ToUtf32( buffer, neededPairs );
		insertIterator = codePoint;

		i += neededPairs;
	}
}



template<typename InserterT> void ConvertToUtf32( char32_t const* source, size_t numCodePoints, InserterT insertIterator )
{
	// Same
	for( size_t i = 0; i < numCodePoints; i++ )
	{
		insertIterator = source[i];
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
