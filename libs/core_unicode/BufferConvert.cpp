#include "stdafx.h"
#include "BufferConvert.h"
#include "InsertConvert.h"

#include "rftl/utility"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Not currently a fully STL-compliant InsertIterator
template<typename CharT>
struct BufferInsertIterator
{
	BufferInsertIterator( CharT* buffer, size_t len )
		: mBuffer( buffer )
		, mBufLen( len )
	{
		//
	}

	BufferInsertIterator& operator=( BufferInsertIterator const& rhs ) = delete;

	BufferInsertIterator& operator=( CharT const& rhs )
	{
		mWriteAttempts++;
		if( mNextInsert < mBufLen )
		{
			mBuffer[mNextInsert] = rhs;
			mNextInsert++;
		}
		return *this;
	}

	BufferInsertIterator& operator=( CharT && rhs )
	{
		mWriteAttempts++;
		if( mNextInsert < mBufLen )
		{
			mBuffer[mNextInsert] = rftl::move( rhs );
			mNextInsert++;
		}
		return *this;
	}

	CharT* const mBuffer;
	size_t const mBufLen;
	size_t mNextInsert = 0;
	size_t mWriteAttempts = 0;
};

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToASCII( char* dest, size_t destLen, char const* source, size_t numBytes )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToASCII( source, numBytes, iter );
	return iter.mWriteAttempts;
}



size_t ConvertToASCII( char * dest, size_t destLen, char16_t const * source, size_t numPairs )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToASCII( source, numPairs, iter );
	return iter.mWriteAttempts;
}



size_t ConvertToASCII( char * dest, size_t destLen, char32_t const * source, size_t numCodePoints )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToASCII( source, numCodePoints, iter );
	return iter.mWriteAttempts;
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf8( char * dest, size_t destLen, char const * source, size_t numBytes )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf8( source, numBytes, iter );
	return iter.mWriteAttempts;
}



size_t ConvertToUtf8( char * dest, size_t destLen, char16_t const * source, size_t numPairs )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf8( source, numPairs, iter );
	return iter.mWriteAttempts;
}



size_t ConvertToUtf8( char * dest, size_t destLen, char32_t const * source, size_t numCodePoints )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf8( source, numCodePoints, iter );
	return iter.mWriteAttempts;
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf16( char16_t * dest, size_t destLen, char const * source, size_t numBytes )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf16( source, numBytes, iter );
	return iter.mWriteAttempts;
}



size_t ConvertToUtf16( char16_t * dest, size_t destLen, char16_t const * source, size_t numPairs )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf16( source, numPairs, iter );
	return iter.mWriteAttempts;
}



size_t ConvertToUtf16( char16_t * dest, size_t destLen, char32_t const * source, size_t numCodePoints )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf16( source, numCodePoints, iter );
	return iter.mWriteAttempts;
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf32( char32_t * dest, size_t destLen, char const * source, size_t numBytes )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf32( source, numBytes, iter );
	return iter.mWriteAttempts;
}



size_t ConvertToUtf32( char32_t * dest, size_t destLen, char16_t const * source, size_t numPairs )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf32( source, numPairs, iter );
	return iter.mWriteAttempts;
}



size_t ConvertToUtf32( char32_t * dest, size_t destLen, char32_t const * source, size_t numCodePoints )
{
	BufferInsertIterator iter( dest, destLen );
	ConvertToUtf32( source, numCodePoints, iter );
	return iter.mWriteAttempts;
}

///////////////////////////////////////////////////////////////////////////////
}}
