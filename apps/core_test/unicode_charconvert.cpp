#include "stdafx.h"

#include "core_unicode/CharConvert.h"

#include "core_math/math_casts.h"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

// GTest can't handle unicode
#define ASSERT_CHAR( A, B ) \
	ASSERT_EQ( ::RF::math::char_integer_bitcast( A ), ::RF::math::char_integer_bitcast( A ) )

///////////////////////////////////////////////////////////////////////////////

TEST( CharConvert, Ascii )
{
	ASSERT_TRUE( IsValidAscii( 'a' ) );
	ASSERT_TRUE( IsValidAscii( u8'a' ) );
	ASSERT_TRUE( IsValidAscii( u'a' ) );
	ASSERT_TRUE( IsValidAscii( U'a' ) );

	ASSERT_FALSE( IsValidAscii( static_cast<char>( 128 ) ) );
	ASSERT_FALSE( IsValidAscii( static_cast<char8_t>( 128 ) ) );
	ASSERT_FALSE( IsValidAscii( static_cast<char16_t>( 128 ) ) );
	ASSERT_FALSE( IsValidAscii( static_cast<char32_t>( 128 ) ) );
}



TEST( CharConvert, Empty )
{
	ASSERT_CHAR( ConvertSingleUtf8ToUtf32( u8"" ), U'\0' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"" ), U'\0' );
}



TEST( CharConvert, Null )
{
	ASSERT_CHAR( ConvertSingleUtf8ToUtf32( rftl::u8string_view( u8"\0", 1 ) ), U'\0' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( rftl::u16string_view( u"\0", 1 ) ), U'\0' );
}



TEST( CharConvert, ToUtf8 )
{
	char8_t temp[4] = {};
	size_t numBytes = 0;
	{
		numBytes = ConvertSingleUtf32ToUtf8( U'\x10000', temp );
		ASSERT_EQ( numBytes, 4 );
		ASSERT_CHAR( temp[0], u8'\xf0' );
		ASSERT_CHAR( temp[1], u8'\x90' );
		ASSERT_CHAR( temp[2], u8'\x80' );
		ASSERT_CHAR( temp[3], u8'\x80' );
	}
	{
		numBytes = ConvertSingleUtf32ToUtf8( U'\x10ffff', temp );
		ASSERT_EQ( numBytes, 4 );
		ASSERT_CHAR( temp[0], u8'\xf4' );
		ASSERT_CHAR( temp[1], u8'\x8f' );
		ASSERT_CHAR( temp[2], u8'\xbf' );
		ASSERT_CHAR( temp[3], u8'\xbf' );
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
	ASSERT_EQ( NumBytesExpectedInUtf8( u8'a' ), 1 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xffff' ), 1 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xd800' ), 2 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xdbff' ), 2 );

	ASSERT_CHAR( ConvertSingleUtf8ToUtf32( u8"a" ), U'a' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"\xffff" ), U'\xffff' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"\xd800\xdc00" ), U'\x10000' );
	ASSERT_CHAR( ConvertSingleUtf16ToUtf32( u"\xdbff\xdfff" ), U'\x10ffff' );
}



TEST( CharConvert, InvalidUTF8 )
{
	// Trying to cram raw encodings into a UTF-8 literal triggers warning C5321
	char8_t asUtf8 = static_cast<char8_t>( '\xff' );

	ASSERT_EQ( NumBytesExpectedInUtf8( asUtf8 ), 0 );
}



TEST( CharConvert, InvalidUTF16 )
{
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xdc00' ), 0 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xdfff' ), 0 );
}

///////////////////////////////////////////////////////////////////////////////
}
