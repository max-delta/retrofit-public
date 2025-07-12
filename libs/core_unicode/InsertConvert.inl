#include "stdafx.h"
#include "InsertConvert.h"

#include "core_unicode/CharConvert.h"
#include "core/rf_assert.h"

#include "rftl/utility"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT, typename InserterT>
void CollapseToAscii( rftl::basic_string_view<CharT> source, InserterT& insertIterator )
{
	for( CharT const& in : source )
	{
		if( IsValidAscii( in ) )
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

template<typename InserterT> void ConvertSingleUtf32ToUtf8( char32_t codePoint, InserterT& insertIterator )
{
	char8_t temp[4] = {};
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
			insertIterator = u8'?';
			break;
	}
}



template<typename InserterT> void ConvertSingleUtf32ToUtf16( char32_t codePoint, InserterT& insertIterator )
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

template<typename InserterT> void ConvertToASCII( rftl::string_view source, InserterT& insertIterator )
{
	details::CollapseToAscii( source, insertIterator );
}



template<typename InserterT> void ConvertToASCII( rftl::u8string_view source, InserterT& insertIterator )
{
	details::CollapseToAscii( source, insertIterator );
}



template<typename InserterT> void ConvertToASCII( rftl::u16string_view source, InserterT& insertIterator )
{
	details::CollapseToAscii( source, insertIterator );
}



template<typename InserterT> void ConvertToASCII( rftl::u32string_view source, InserterT& insertIterator )
{
	details::CollapseToAscii( source, insertIterator );
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf8( rftl::string_view source, InserterT& insertIterator )
{
	// Same if top bit isn't set
	for( char const& ch : source )
	{
		if( IsValidAscii( ch ) )
		{
			insertIterator = static_cast<char8_t>( ch );
		}
		else
		{
			insertIterator = u8'?';
		}
	}
}



template<typename InserterT>
void ConvertToUtf8( rftl::u8string_view source, InserterT& insertIterator )
{
	// Same
	for( char8_t const& ch : source )
	{
		insertIterator = ch;
	}
}



template<typename InserterT> void ConvertToUtf8( rftl::u16string_view source, InserterT& insertIterator )
{
	// Transition through UTF-32
	rftl::u16string_view view = source;
	while( view.empty() == false )
	{
		size_t const remainingPairs = view.size();
		size_t const neededPairs = NumPairsExpectedInUtf8( view.front() );

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
		char32_t const codePoint = ConvertSingleUtf16ToUtf32( view.substr( 0, neededPairs ) );

		// Shrink
		ConvertSingleUtf32ToUtf8( codePoint, insertIterator );

		view = view.substr( neededPairs );
	}
}



template<typename InserterT> void ConvertToUtf8( rftl::u32string_view source, InserterT& insertIterator )
{
	// Shrink
	for( char32_t const& codePoint : source )
	{
		ConvertSingleUtf32ToUtf8( codePoint, insertIterator );
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf16( rftl::string_view source, InserterT& insertIterator )
{
	// Transition through UTF-32
	for( char const& ch : source )
	{
		if( IsValidAscii( ch ) == false )
		{
			insertIterator = u'?';
			continue;
		}
		char8_t const asUtf8 = static_cast<char8_t const>( ch );

		// Expand
		char32_t const codePoint = ConvertSingleUtf8ToUtf32( rftl::u8string_view( &asUtf8, 1 ) );

		// Shrink
		ConvertSingleUtf32ToUtf16( codePoint, insertIterator );
	}
}



template<typename InserterT>
void ConvertToUtf16( rftl::u8string_view source, InserterT& insertIterator )
{
	// Transition through UTF-32
	rftl::u8string_view view = source;
	while( view.empty() == false )
	{
		size_t const remainingBytes = view.size();
		size_t const neededBytes = NumBytesExpectedInUtf8( view.front() );

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
		char32_t const codePoint = ConvertSingleUtf8ToUtf32( view.substr( 0, neededBytes ) );

		// Shrink
		ConvertSingleUtf32ToUtf16( codePoint, insertIterator );

		view = view.substr( neededBytes );
	}
}



template<typename InserterT> void ConvertToUtf16( rftl::u16string_view source, InserterT& insertIterator )
{
	// Same
	for( char16_t const& pair : source )
	{
		insertIterator = pair;
	}
}



template<typename InserterT> void ConvertToUtf16( rftl::u32string_view source, InserterT& insertIterator )
{
	// Shrink
	for( char32_t const& codePoint : source )
	{
		ConvertSingleUtf32ToUtf16( codePoint, insertIterator );
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf32( rftl::string_view source, InserterT& insertIterator )
{
	// Expand
	for( char const& ch : source )
	{
		if( IsValidAscii( ch ) == false )
		{
			insertIterator = U'?';
			continue;
		}
		char8_t const asUtf8 = static_cast<char8_t const>( ch );

		char32_t const codePoint = ConvertSingleUtf8ToUtf32( rftl::u8string_view( &asUtf8, 1 ) );
		insertIterator = codePoint;
	}
}



template<typename InserterT>
void ConvertToUtf32( rftl::u8string_view source, InserterT& insertIterator )
{
	// Expand
	rftl::u8string_view view = source;
	while( view.empty() == false )
	{
		size_t const remainingBytes = view.size();
		size_t const neededBytes = NumBytesExpectedInUtf8( view.front() );

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

		char32_t const codePoint = ConvertSingleUtf8ToUtf32( view.substr( 0, neededBytes ) );
		insertIterator = codePoint;

		view = view.substr( neededBytes );
	}
}



template<typename InserterT> void ConvertToUtf32( rftl::u16string_view source, InserterT& insertIterator )
{
	// Expand
	rftl::u16string_view view = source;
	while( view.empty() == false )
	{
		size_t const remainingPairs = view.size();
		size_t const neededPairs = NumPairsExpectedInUtf8( view.front() );

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

		char32_t const codePoint = ConvertSingleUtf16ToUtf32( view.substr( 0, neededPairs ) );
		insertIterator = codePoint;

		view = view.substr( neededPairs );
	}
}



template<typename InserterT> void ConvertToUtf32( rftl::u32string_view source, InserterT& insertIterator )
{
	// Same
	for( char32_t const& codePoint : source )
	{
		insertIterator = codePoint;
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertSingleUtf32ToUtf8( char32_t codePoint, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertSingleUtf32ToUtf8( codePoint, discard );
}



template<typename InserterT> void ConvertSingleUtf32ToUtf16( char32_t codePoint, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertSingleUtf32ToUtf16( codePoint, discard );
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToASCII( rftl::string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToASCII( source, discard );
}



template<typename InserterT>
void ConvertToASCII( rftl::u8string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToASCII( source, discard );
}



template<typename InserterT> void ConvertToASCII( rftl::u16string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToASCII( source, discard );
}



template<typename InserterT> void ConvertToASCII( rftl::u32string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToASCII( source, discard );
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf8( rftl::string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf8( source, discard );
}



template<typename InserterT>
void ConvertToUtf8( rftl::u8string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf8( source, discard );
}



template<typename InserterT> void ConvertToUtf8( rftl::u16string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf8( source, discard );
}



template<typename InserterT> void ConvertToUtf8( rftl::u32string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf8( source, discard );
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf16( rftl::string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf16( source, discard );
}



template<typename InserterT>
void ConvertToUtf16( rftl::u8string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf16( source, discard );
}



template<typename InserterT> void ConvertToUtf16( rftl::u16string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf16( source, discard );
}



template<typename InserterT> void ConvertToUtf16( rftl::u32string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf16( source, discard );
}

///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertToUtf32( rftl::string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf32( source, discard );
}



template<typename InserterT>
void ConvertToUtf32( rftl::u8string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf32( source, discard );
}



template<typename InserterT> void ConvertToUtf32( rftl::u16string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf32( source, discard );
}



template<typename InserterT> void ConvertToUtf32( rftl::u32string_view source, InserterT&& insertIterator )
{
	InserterT discard = rftl::move( insertIterator );
	ConvertToUtf32( source, discard );
}

///////////////////////////////////////////////////////////////////////////////
}
