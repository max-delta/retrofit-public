#include "stdafx.h"
#include "StringConvert.h"

#include "core/macros.h"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename CharT>
std::string CollapseToAscii( std::basic_string<CharT> const& source )
{
	std::string retVal;
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

}
///////////////////////////////////////////////////////////////////////////////

std::string ConvertToASCII( std::string const & source )
{
	return details::CollapseToAscii( source );
}



std::string ConvertToASCII( std::u16string const & source )
{
	return details::CollapseToAscii( source );
}



std::string ConvertToASCII( std::u32string const & source )
{
	return details::CollapseToAscii( source );
}

///////////////////////////////////////////////////////////////////////////////

std::string ConvertToUtf8( std::string const & source )
{
	// Same
	return source;
}



std::string ConvertToUtf8( std::u16string const & source )
{
	// Shrink
	// HACK: Transition through UTF-32
	return ConvertToUtf8( ConvertToUtf32( source ) );
}



std::string ConvertToUtf8( std::u32string const & source )
{
	// Shrink
	std::string retVal;
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

std::u16string ConvertToUtf16( std::string const & source )
{
	// Expand
	// HACK: Transition through UTF-32
	return ConvertToUtf16( ConvertToUtf32( source ) );
}



std::u16string ConvertToUtf16( std::u16string const & source )
{
	// Same
	return source;
}



std::u16string ConvertToUtf16( std::u32string const & source )
{
	// Shrink
	std::u16string retVal;
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
			char16_t const lowSurrogate = high10bit + 0xdc00u;
			RF_ASSERT( highSurrogate >= 0xdc00 );
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

std::u32string ConvertToUtf32( std::string const & source )
{
	// Expand
	RF_DBGFAIL_MSG( "TODO" );
	return std::u32string();
}



std::u32string ConvertToUtf32( std::u16string const & source )
{
	// Expand
	RF_DBGFAIL_MSG( "TODO" );
	return std::u32string();
}



std::u32string ConvertToUtf32( std::u32string const & source )
{
	return source;
}

///////////////////////////////////////////////////////////////////////////////
}}
