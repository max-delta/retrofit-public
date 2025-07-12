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
	BufferInsertIterator( rftl::span<CharT> buffer, size_t* numWriteAtempts )
		: mBuffer( buffer )
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
		if( mNextInsert < mBuffer.size() )
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
		if( mNextInsert < mBuffer.size() )
		{
			mBuffer[mNextInsert] = rftl::move( rhs );
			mNextInsert++;
		}
		return *this;
	}

	rftl::span<CharT> const mBuffer;
	size_t* const mWriteAttempts;
	size_t mNextInsert = 0;
};


template<typename DestT, typename SourceT>
using ConvertToInsertSig = void( rftl::basic_string_view<SourceT>, BufferInsertIterator<DestT>&& );

template<typename DestT, typename CallableT, typename SourceT>
size_t ConvertTo( CallableT const& callable, rftl::span<DestT> dest, rftl::basic_string_view<SourceT> source )
{
	size_t writeAttempts = 0;
	BufferInsertIterator<DestT> iter( dest, &writeAttempts );
	callable( source, rftl::move( iter ) );
	return writeAttempts;
}



template<typename SourceT>
size_t ConvertToASCII( rftl::span<char> dest, rftl::basic_string_view<SourceT> source )
{
	ConvertToInsertSig<char, SourceT>* const call = unicode::ConvertToASCII;
	return ConvertTo<char>( call, dest, source );
}

template<typename SourceT>
size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::basic_string_view<SourceT> source )
{
	ConvertToInsertSig<char8_t, SourceT>* const call = unicode::ConvertToUtf8;
	return ConvertTo<char8_t>( call, dest, source );
}

template<typename SourceT>
size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::basic_string_view<SourceT> source )
{
	ConvertToInsertSig<char16_t, SourceT>* const call = unicode::ConvertToUtf16;
	return ConvertTo<char16_t>( call, dest, source );
}

template<typename SourceT>
size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::basic_string_view<SourceT> source )
{
	ConvertToInsertSig<char32_t, SourceT>* const call = unicode::ConvertToUtf32;
	return ConvertTo<char32_t>( call, dest, source );
}

}
///////////////////////////////////////////////////////////////////////////////

size_t ConvertToASCII( rftl::span<char> dest, rftl::string_view source )
{
	return details::ConvertToASCII( dest, source );
}

size_t ConvertToASCII( rftl::span<char> dest, rftl::u8string_view source )
{
	return details::ConvertToASCII( dest, source );
}

size_t ConvertToASCII( rftl::span<char> dest, rftl::u16string_view source )
{
	return details::ConvertToASCII( dest, source );
}

size_t ConvertToASCII( rftl::span<char> dest, rftl::u32string_view source )
{
	return details::ConvertToASCII( dest, source );
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::string_view source )
{
	return details::ConvertToUtf8( dest, source );
}

size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::u8string_view source )
{
	return details::ConvertToUtf8( dest, source );
}

size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::u16string_view source )
{
	return details::ConvertToUtf8( dest, source );
}

size_t ConvertToUtf8( rftl::span<char8_t> dest, rftl::u32string_view source )
{
	return details::ConvertToUtf8( dest, source );
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::string_view source )
{
	return details::ConvertToUtf16( dest, source );
}

size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::u8string_view source )
{
	return details::ConvertToUtf16( dest, source );
}

size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::u16string_view source )
{
	return details::ConvertToUtf16( dest, source );
}

size_t ConvertToUtf16( rftl::span<char16_t> dest, rftl::u32string_view source )
{
	return details::ConvertToUtf16( dest, source );
}

///////////////////////////////////////////////////////////////////////////////

size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::string_view source )
{
	return details::ConvertToUtf32( dest, source );
}

size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::u8string_view source )
{
	return details::ConvertToUtf32( dest, source );
}

size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::u16string_view source )
{
	return details::ConvertToUtf32( dest, source );
}

size_t ConvertToUtf32( rftl::span<char32_t> dest, rftl::u32string_view source )
{
	return details::ConvertToUtf32( dest, source );
}

///////////////////////////////////////////////////////////////////////////////
}
