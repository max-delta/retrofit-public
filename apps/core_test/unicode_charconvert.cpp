#include "stdafx.h"

#include "core_unicode/CharConvert.h"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

TEST( CharConvert, Empty )
{
	ASSERT_EQ( ConvertSingleUtf8ToUtf32( "", 0 ), U'\0' );
	ASSERT_EQ( ConvertSingleUtf16ToUtf32( u"", 0 ), U'\0' );
}



TEST( CharConvert, Null )
{
	ASSERT_EQ( ConvertSingleUtf8ToUtf32( "\0", 1 ), U'\0' );
	ASSERT_EQ( ConvertSingleUtf16ToUtf32( u"\0", 1 ), U'\0' );
}



TEST( CharConvert, ToUtf32 )
{
	ASSERT_EQ( NumBytesExpectedInUtf8( 'a' ), 1 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xffff' ), 1 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xd800' ), 2 );
	ASSERT_EQ( NumPairsExpectedInUtf8( u'\xdbff' ), 2 );

	ASSERT_EQ( ConvertSingleUtf8ToUtf32( "a", 1 ), U'a' );
	ASSERT_EQ( ConvertSingleUtf16ToUtf32( u"\xffff", 1 ), U'\xffff' );
	ASSERT_EQ( ConvertSingleUtf16ToUtf32( u"\xd800\xdc00", 2 ), U'\x10000' );
	ASSERT_EQ( ConvertSingleUtf16ToUtf32( u"\xdbff\xdfff", 2 ), U'\x10ffff' );
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
}}
