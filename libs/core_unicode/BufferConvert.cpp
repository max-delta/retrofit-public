#include "stdafx.h"
#include "BufferConvert.h"
#include "InsertConvert.h"

#include "rftl/utility"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// NOTE: Not currently a fully STL-compliant InsertIterator
template<typename CharT>
struct BufferInsertIterator
{
	BufferInsertIterator( CharT* buffer, size_t len, size_t* numWriteAtempts )
		: mBuffer( buffer )
		, mBufLen( len )
		, mWriteAttempts( numWriteAtempts )
	{
		//
	}

	BufferInsertIterator( BufferInsertIterator const& rhs ) = default;

	BufferInsertIterator& operator=( BufferInsertIterator const& rhs ) = delete;

	BufferInsertIterator& operator=( CharT const& rhs )
	{
		if( mWriteAttempts != nullptr )
		{
			( *mWriteAttempts )++;
		}
		if( mNextInsert < mBufLen )
		{
			mBuffer[mNextInsert] = rhs;
			mNextInsert++;
		}
		return *this;
	}

	BufferInsertIterator& operator=( CharT&& rhs )
	{
		if( mWriteAttempts != nullptr )
		{
			( *mWriteAttempts )++;
		}
		if( mNextInsert < mBufLen )
		{
			mBuffer[mNextInsert] = rftl::move( rhs );
			mNextInsert++;
		}
		return *this;
	}

	CharT* const mBuffer;
	size_t const mBufLen;
	size_t* const mWriteAttempts;
	size_t mNextInsert = 0;
};


template<typename DestT, typename SourceT>
using ConvertToInsertSig = void( rftl::basic_string_view<SourceT>, BufferInsertIterator<DestT>&& );

template<typename DestT, typename CallableT, typename SourceT>
size_t ConvertTo( CallableT const& callable, DestT* dest, size_t destLen, rftl::basic_string_view<SourceT> source )
{
	size_t writeAttempts = 0;
	BufferInsertIterator<DestT> iter( dest, destLen, &writeAttempts );
	callable( source, rftl::move( iter ) );
	return writeAttempts;
}



template<typename SourceT>
size_t ConvertToASCII( char* dest, size_t destLen, rftl::basic_string_view<SourceT> source )
{
	ConvertToInsertSig<char, SourceT>* const call = unicode::ConvertToASCII;
	return ConvertTo<char>( call, dest, destLen, source );
}

template<typename SourceT>
size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::basic_string_view<SourceT> source )
{
	ConvertToInsertSig<char8_t, SourceT>* const call = unicode::ConvertToUtf8;
	return ConvertTo<char8_t>( call, dest, destLen, source );
}

template<typename SourceT>
size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::basic_string_view<SourceT> source )
{
	ConvertToInsertSig<char16_t, SourceT>* const call = unicode::ConvertToUtf16;
	return ConvertTo<char16_t>( call, dest, destLen, source );
}

template<typename SourceT>
size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::basic_string_view<SourceT> source )
{
	ConvertToInsertSig<char32_t, SourceT>* const call = unicode::ConvertToUtf32;
	return ConvertTo<char32_t>( call, dest, destLen, source );
}

}
///////////////////////////////////////////////////////////////////////////////

size_t ConvertToASCII( char* dest, size_t destLen, rftl::string_view source )
{
	return details::ConvertToASCII( dest, destLen, source );
}

size_t ConvertToASCII( char* dest, size_t destLen, rftl::u8string_view source )
{
	return details::ConvertToASCII( dest, destLen, source );
}

size_t ConvertToASCII( char* dest, size_t destLen, rftl::u16string_view source )
{
	return details::ConvertToASCII( dest, destLen, source );
}

size_t ConvertToASCII( char* dest, size_t destLen, rftl::u32string_view source )
{
	return details::ConvertToASCII( dest, destLen, source );
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::string_view source )
{
	return details::ConvertToUtf8( dest, destLen, source );
}

size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::u8string_view source )
{
	return details::ConvertToUtf8( dest, destLen, source );
}

size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::u16string_view source )
{
	return details::ConvertToUtf8( dest, destLen, source );
}

size_t ConvertToUtf8( char8_t* dest, size_t destLen, rftl::u32string_view source )
{
	return details::ConvertToUtf8( dest, destLen, source );
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::string_view source )
{
	return details::ConvertToUtf16( dest, destLen, source );
}

size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::u8string_view source )
{
	return details::ConvertToUtf16( dest, destLen, source );
}

size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::u16string_view source )
{
	return details::ConvertToUtf16( dest, destLen, source );
}

size_t ConvertToUtf16( char16_t* dest, size_t destLen, rftl::u32string_view source )
{
	return details::ConvertToUtf16( dest, destLen, source );
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::string_view source )
{
	return details::ConvertToUtf32( dest, destLen, source );
}

size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::u8string_view source )
{
	return details::ConvertToUtf32( dest, destLen, source );
}

size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::u16string_view source )
{
	return details::ConvertToUtf32( dest, destLen, source );
}

size_t ConvertToUtf32( char32_t* dest, size_t destLen, rftl::u32string_view source )
{
	return details::ConvertToUtf32( dest, destLen, source );
}

///////////////////////////////////////////////////////////////////////////////
}
