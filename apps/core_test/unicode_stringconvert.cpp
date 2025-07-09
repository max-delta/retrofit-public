#include "stdafx.h"

#include "core_unicode/StringConvert.h"

#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

// GTest can't handle unicode
#define ASSERT_STR( A, B ) \
	ASSERT_TRUE( A == B )

namespace details {
// Trying to cram raw encodings into a UTF-8 literal triggers warning C5321
static rftl::u8string FromBytes( rftl::string_view bytes )
{
	return rftl::u8string( reinterpret_cast<char8_t const*>( bytes.data() ), bytes.size() );
}
}

///////////////////////////////////////////////////////////////////////////////

TEST( StringConvert, Empty )
{
	ASSERT_STR( ConvertToASCII( "" ), "" );
	ASSERT_STR( ConvertToASCII( u8"" ), "" );
	ASSERT_STR( ConvertToASCII( u"" ), "" );
	ASSERT_STR( ConvertToASCII( U"" ), "" );

	ASSERT_STR( ConvertToUtf8( "" ), u8"" );
	ASSERT_STR( ConvertToUtf8( u8"" ), u8"" );
	ASSERT_STR( ConvertToUtf8( u"" ), u8"" );
	ASSERT_STR( ConvertToUtf8( U"" ), u8"" );

	ASSERT_STR( ConvertToUtf16( "" ), u"" );
	ASSERT_STR( ConvertToUtf16( u8"" ), u"" );
	ASSERT_STR( ConvertToUtf16( u"" ), u"" );
	ASSERT_STR( ConvertToUtf16( U"" ), u"" );

	ASSERT_STR( ConvertToUtf32( "" ), U"" );
	ASSERT_STR( ConvertToUtf32( u8"" ), U"" );
	ASSERT_STR( ConvertToUtf32( u"" ), U"" );
	ASSERT_STR( ConvertToUtf32( U"" ), U"" );
}



TEST( StringConvert, Null )
{
	ASSERT_STR( ConvertToASCII( "\0" ), "\0" );
	ASSERT_STR( ConvertToASCII( u8"\0" ), "\0" );
	ASSERT_STR( ConvertToASCII( u"\0" ), "\0" );
	ASSERT_STR( ConvertToASCII( U"\0" ), "\0" );

	ASSERT_STR( ConvertToUtf8( "\0" ), u8"\0" );
	ASSERT_STR( ConvertToUtf8( u8"\0" ), u8"\0" );
	ASSERT_STR( ConvertToUtf8( u"\0" ), u8"\0" );
	ASSERT_STR( ConvertToUtf8( U"\0" ), u8"\0" );

	ASSERT_STR( ConvertToUtf16( "\0" ), u"\0" );
	ASSERT_STR( ConvertToUtf16( u8"\0" ), u"\0" );
	ASSERT_STR( ConvertToUtf16( u"\0" ), u"\0" );
	ASSERT_STR( ConvertToUtf16( U"\0" ), u"\0" );

	ASSERT_STR( ConvertToUtf32( "\0" ), U"\0" );
	ASSERT_STR( ConvertToUtf32( u8"\0" ), U"\0" );
	ASSERT_STR( ConvertToUtf32( u"\0" ), U"\0" );
	ASSERT_STR( ConvertToUtf32( U"\0" ), U"\0" );
}



TEST( StringConvert, ToAscii )
{
	ASSERT_STR( ConvertToASCII( "ab" ), "ab" );
	ASSERT_STR( ConvertToASCII( u8"ab" ), "ab" );
	ASSERT_STR( ConvertToASCII( u"\xffff" ), "?" );
	ASSERT_STR( ConvertToASCII( u"\xd800\xdc00" ), "??" ); // Questionable (make '?' instead?)
	ASSERT_STR( ConvertToASCII( u"\xdbff\xdfff" ), "??" ); // Questionable (make '?' instead?)
	ASSERT_STR( ConvertToASCII( U"\x10000" ), "?" );
	ASSERT_STR( ConvertToASCII( U"\x10ffff" ), "?" );
}



TEST( StringConvert, ToUtf8 )
{
	ASSERT_STR( ConvertToUtf8( "ab" ), u8"ab" );
	ASSERT_STR( ConvertToUtf8( u8"ab" ), u8"ab" );
	ASSERT_STR( ConvertToUtf8( u"\xffff" ), details::FromBytes( "\xef\xbf\xbf" ) );
	ASSERT_STR( ConvertToUtf8( u"\xd800\xdc00" ), details::FromBytes( "\xf0\x90\x80\x80" ) );
	ASSERT_STR( ConvertToUtf8( u"\xdbff\xdfff" ), details::FromBytes( "\xf4\x8f\xbf\xbf" ) );
	ASSERT_STR( ConvertToUtf8( U"\x10000" ), details::FromBytes( "\xf0\x90\x80\x80" ) );
	ASSERT_STR( ConvertToUtf8( U"\x10ffff" ), details::FromBytes( "\xf4\x8f\xbf\xbf" ) );
}



TEST( StringConvert, ToUtf16 )
{
	ASSERT_STR( ConvertToUtf16( "ab" ), u"ab" );
	ASSERT_STR( ConvertToUtf16( u"\xffff" ), u"\xffff" );
	ASSERT_STR( ConvertToUtf16( u"\xd800\xdc00" ), u"\xd800\xdc00" );
	ASSERT_STR( ConvertToUtf16( u"\xdbff\xdfff" ), u"\xdbff\xdfff" );
	ASSERT_STR( ConvertToUtf16( U"\x10000" ), u"\xd800\xdc00" );
	ASSERT_STR( ConvertToUtf16( U"\x10ffff" ), u"\xdbff\xdfff" );
}



TEST( StringConvert, ToUtf32 )
{
	ASSERT_STR( ConvertToUtf32( "ab" ), U"ab" );
	ASSERT_STR( ConvertToUtf32( u8"ab" ), U"ab" );
	ASSERT_STR( ConvertToUtf32( u"\xffff" ), U"\xffff" );
	ASSERT_STR( ConvertToUtf32( u"\xd800\xdc00" ), U"\x10000" );
	ASSERT_STR( ConvertToUtf32( u"\xdbff\xdfff" ), U"\x10ffff" );
	ASSERT_STR( ConvertToUtf32( U"\x10000" ), U"\x10000" );
	ASSERT_STR( ConvertToUtf32( U"\x10ffff" ), U"\x10ffff" );
}



TEST( StringConvert, InvalidAscii )
{
	ASSERT_STR( ConvertToASCII( "\xff" ), "?" );
}



TEST( StringConvert, InvalidUTF8 )
{
	ASSERT_STR( ConvertToASCII( "\xff" ), "?" );
	ASSERT_STR( ConvertToUtf8( "\xff" ), u8"?" );
	ASSERT_STR( ConvertToUtf16( "\xff" ), u"?" );
	ASSERT_STR( ConvertToUtf32( "\xff" ), U"?" );
}

///////////////////////////////////////////////////////////////////////////////
}
