#include "stdafx.h"

#include "core_unicode/StringConvert.h"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

TEST( StringConvert, Empty )
{
	ASSERT_EQ( ConvertToASCII( "" ), "" );
	ASSERT_EQ( ConvertToASCII( u"" ), "" );
	ASSERT_EQ( ConvertToASCII( U"" ), "" );

	ASSERT_EQ( ConvertToUtf8( "" ), "" );
	ASSERT_EQ( ConvertToUtf8( u"" ), "" );
	ASSERT_EQ( ConvertToUtf8( U"" ), "" );

	ASSERT_EQ( ConvertToUtf16( "" ), u"" );
	ASSERT_EQ( ConvertToUtf16( u"" ), u"" );
	ASSERT_EQ( ConvertToUtf16( U"" ), u"" );

	ASSERT_EQ( ConvertToUtf32( "" ), U"" );
	ASSERT_EQ( ConvertToUtf32( u"" ), U"" );
	ASSERT_EQ( ConvertToUtf32( U"" ), U"" );
}



TEST( StringConvert, Null )
{
	ASSERT_EQ( ConvertToASCII( "\0" ), "\0" );
	ASSERT_EQ( ConvertToASCII( u"\0" ), "\0" );
	ASSERT_EQ( ConvertToASCII( U"\0" ), "\0" );

	ASSERT_EQ( ConvertToUtf8( "\0" ), "\0" );
	ASSERT_EQ( ConvertToUtf8( u"\0" ), "\0" );
	ASSERT_EQ( ConvertToUtf8( U"\0" ), "\0" );

	ASSERT_EQ( ConvertToUtf16( "\0" ), u"\0" );
	ASSERT_EQ( ConvertToUtf16( u"\0" ), u"\0" );
	ASSERT_EQ( ConvertToUtf16( U"\0" ), u"\0" );

	ASSERT_EQ( ConvertToUtf32( "\0" ), U"\0" );
	ASSERT_EQ( ConvertToUtf32( u"\0" ), U"\0" );
	ASSERT_EQ( ConvertToUtf32( U"\0" ), U"\0" );
}



TEST( StringConvert, ToAscii )
{
	ASSERT_EQ( ConvertToASCII( "a" ), "a" );
	ASSERT_EQ( ConvertToASCII( u"\xffff" ), "?" );
	ASSERT_EQ( ConvertToASCII( u"\xd800\xdc00" ), "??" ); // Questionable (make '?' instead?)
	ASSERT_EQ( ConvertToASCII( u"\xdbff\xdfff" ), "??" ); // Questionable (make '?' instead?)
	ASSERT_EQ( ConvertToASCII( U"\x10000" ), "?" );
	ASSERT_EQ( ConvertToASCII( U"\x10ffff" ), "?" );
}



TEST( StringConvert, ToUtf8 )
{
	ASSERT_EQ( ConvertToUtf8( "a" ), "a" );
	ASSERT_EQ( ConvertToUtf8( u"\xffff" ), "\xef\xbf\xbf" );
	ASSERT_EQ( ConvertToUtf8( u"\xd800\xdc00" ), "\xf0\x90\x80\x80" );
	ASSERT_EQ( ConvertToUtf8( u"\xdbff\xdfff" ), "\xf4\x8f\xbf\xbf" );
	ASSERT_EQ( ConvertToUtf8( U"\x10000" ), "\xf0\x90\x80\x80" );
	ASSERT_EQ( ConvertToUtf8( U"\x10ffff" ), "\xf4\x8f\xbf\xbf" );
}



TEST( StringConvert, ToUtf16 )
{
	ASSERT_EQ( ConvertToUtf16( "a" ), u"a" );
	ASSERT_EQ( ConvertToUtf16( u"\xffff" ), u"\xffff" );
	ASSERT_EQ( ConvertToUtf16( u"\xd800\xdc00" ), u"\xd800\xdc00" );
	ASSERT_EQ( ConvertToUtf16( u"\xdbff\xdfff" ), u"\xdbff\xdfff" );
	ASSERT_EQ( ConvertToUtf16( U"\x10000" ), u"\xd800\xdc00" );
	ASSERT_EQ( ConvertToUtf16( U"\x10ffff" ), u"\xdbff\xdfff" );
}



TEST( StringConvert, ToUtf32 )
{
	ASSERT_EQ( ConvertToUtf32( "a" ), U"a" );
	ASSERT_EQ( ConvertToUtf32( u"\xffff" ), U"\xffff" );
	ASSERT_EQ( ConvertToUtf32( u"\xd800\xdc00" ), U"\x10000" );
	ASSERT_EQ( ConvertToUtf32( u"\xdbff\xdfff" ), U"\x10ffff" );
	ASSERT_EQ( ConvertToUtf32( U"\x10000" ), U"\x10000" );
	ASSERT_EQ( ConvertToUtf32( U"\x10ffff" ), U"\x10ffff" );
}



TEST( StringConvert, InvalidAscii )
{
	ASSERT_EQ( ConvertToASCII( "\xff" ), "?" );
}



TEST( StringConvert, InvalidUTF8 )
{
	ASSERT_EQ( ConvertToASCII( "\xff" ), "?" );
	ASSERT_EQ( ConvertToUtf8( "\xff" ), "\xff" ); // Questionable (make '?' instead?)
	ASSERT_EQ( ConvertToUtf16( "\xff" ), u"?" );
	ASSERT_EQ( ConvertToUtf32( "\xff" ), U"?" );
}

///////////////////////////////////////////////////////////////////////////////
}}
