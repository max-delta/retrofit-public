#include "stdafx.h"

#include "core_unicode/CharConvert.h"

#include "core_math/math_casts.h"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

// GTest can't handle unicode
#define ASSERT_CHAR( A, B ) \
	ASSERT_EQ( ::RF::math::char_integer_bitcast( A ), ::RF::math::char_integer_bitcast( A ) )

///////////////////////////////////////////////////////////////////////////////

TEST( CharConvert, Empty )
{
	ASSERT_CHAR( ConvertSingleUtf8ToUtf32( "", 0 ), U'\0' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"", 0 ), U'\0' );
}



TEST( CharConvert, Null )
{
	ASSERT_CHAR( ConvertSingleUtf8ToUtf32( "\0", 1 ), U'\0' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"\0", 1 ), U'\0' );
}



TEST( CharConvert, ToUtf8 )
{
	char temp[4] = {};
	size_t numBytes = 0;
	{
		numBytes = ConvertSingleUtf32ToUtf8( U'\x10000', temp );
		ASSERT_EQ( numBytes, 4 );
		ASSERT_CHAR( temp[0], '\xf0' );
		ASSERT_CHAR( temp[1], '\x90' );
		ASSERT_CHAR( temp[2], '\x80' );
		ASSERT_CHAR( temp[3], '\x80' );
	}
	{
		numBytes = ConvertSingleUtf32ToUtf8( U'\x10ffff', temp );
		ASSERT_EQ( numBytes, 4 );
		ASSERT_CHAR( temp[0], '\xf4' );
		ASSERT_CHAR( temp[1], '\x8f' );
		ASSERT_CHAR( temp[2], '\xbf' );
		ASSERT_CHAR( temp[3], '\xbf' );
	}
}



TEST( CharConvert, ToUtf16 )
{
	char16_t temp[2] = {};
	size_t numPairs = 0;
	{
		numPairs = ConvertSingleUtf32ToUtf16( U'\x10000', temp );
		ASSERT_EQ( numPairs, 2 );
		ASSERT_CHAR( temp[0], u'\xd800' );
		ASSERT_CHAR( temp[1], u'\xdc00' );
	}
	{
		numPairs = ConvertSingleUtf32ToUtf16( U'\x10ffff', temp );
		ASSERT_EQ( numPairs, 2 );
		ASSERT_CHAR( temp[0], u'\xdbff' );
		ASSERT_CHAR( temp[1], u'\xdfff' );
	}
}



TEST( CharConvert, ToUtf32 )
{
	ASSERT_EQ( NumBytesExpectedInUtf8( 'a' ), 1 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xffff' ), 1 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xd800' ), 2 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xdbff' ), 2 );

	ASSERT_CHAR( ConvertSingleUtf8ToUtf32( "a", 1 ), U'a' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"\xffff", 1 ), U'\xffff' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"\xd800\xdc00", 2 ), U'\x10000' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"\xdbff\xdfff", 2 ), U'\x10ffff' );
}



TEST( CharConvert, InvalidUTF8 )
{
	ASSERT_EQ( NumBytesExpectedInUtf8( '\xff' ), 0 );
}



TEST( CharConvert, InvalidUTF16 )
{
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xdc00' ), 0 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xdfff' ), 0 );
}

///////////////////////////////////////////////////////////////////////////////
}
