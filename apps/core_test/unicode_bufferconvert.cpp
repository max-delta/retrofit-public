#include "stdafx.h"

#include "core_unicode/BufferConvert.h"

#include "core_math/math_casts.h"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

// GTest can't handle unicode
#define ASSERT_CHAR( A, B ) \
	ASSERT_EQ( ::RF::math::char_integer_bitcast( A ), ::RF::math::char_integer_bitcast( A ) )

///////////////////////////////////////////////////////////////////////////////

TEST( BufferConvert, EmptySource )
{
	ASSERT_EQ( ConvertToASCII( nullptr, 0, "" ), 0 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, u8"" ), 0 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, u"" ), 0 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, U"" ), 0 );

	ASSERT_EQ( ConvertToUtf8( nullptr, 0, "" ), 0 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, u8"" ), 0 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, u"" ), 0 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, U"" ), 0 );

	ASSERT_EQ( ConvertToUtf16( nullptr, 0, "" ), 0 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, u8"" ), 0 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, u"" ), 0 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, U"" ), 0 );

	ASSERT_EQ( ConvertToUtf32( nullptr, 0, "" ), 0 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, u8"" ), 0 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, u"" ), 0 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, U"" ), 0 );
}



TEST( BufferConvert, EmptyDest )
{
	ASSERT_EQ( ConvertToASCII( nullptr, 0, "a" ), 1 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, u8"a" ), 1 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, u"a" ), 1 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, U"a" ), 1 );

	ASSERT_EQ( ConvertToUtf8( nullptr, 0, "a" ), 1 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, u8"a" ), 1 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, u"a" ), 1 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, U"a" ), 1 );

	ASSERT_EQ( ConvertToUtf16( nullptr, 0, "a" ), 1 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, u8"a" ), 1 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, u"a" ), 1 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, U"a" ), 1 );

	ASSERT_EQ( ConvertToUtf32( nullptr, 0, "a" ), 1 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, u8"a" ), 1 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, u"a" ), 1 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, U"a" ), 1 );
}



TEST( BufferConvert, Null )
{
	char c;
	char8_t u8;
	char16_t u;
	char32_t U;

	// NOTE: Passing in strings with two null terminators, so the call will
	//  report that two writes were attempted, but only one will actually
	//  get applied
	RF_TODO_ANNOTATION( "Add tests to make sure that the second null isn't actually written" );

	c = 'a';
	ASSERT_EQ( ConvertToASCII( &c, 1, rftl::string_view( "\0", 2 ) ), 2 );
	ASSERT_CHAR( c, '\0' );
	c = 'a';
	ASSERT_EQ( ConvertToASCII( &c, 1, rftl::u8string_view( u8"\0", 2 ) ), 2 );
	ASSERT_CHAR( c, '\0' );
	c = 'a';
	ASSERT_EQ( ConvertToASCII( &c, 1, rftl::u16string_view( u"\0", 2 ) ), 2 );
	ASSERT_CHAR( c, '\0' );
	c = 'a';
	ASSERT_EQ( ConvertToASCII( &c, 1, rftl::u32string_view( U"\0", 2 ) ), 2 );
	ASSERT_CHAR( c, '\0' );

	u8 = 'a';
	ASSERT_EQ( ConvertToUtf8( &u8, 1, rftl::string_view( "\0", 2 ) ), 2 );
	ASSERT_CHAR( u8, u8'\0' );
	u8 = 'a';
	ASSERT_EQ( ConvertToUtf8( &u8, 1, rftl::u8string_view( u8"\0", 2 ) ), 2 );
	ASSERT_CHAR( u8, u8'\0' );
	u8 = 'a';
	ASSERT_EQ( ConvertToUtf8( &u8, 1, rftl::u16string_view( u"\0", 2 ) ), 2 );
	ASSERT_CHAR( u8, u8'\0' );
	u8 = 'a';
	ASSERT_EQ( ConvertToUtf8( &u8, 1, rftl::u32string_view( U"\0", 2 ) ), 2 );
	ASSERT_CHAR( u8, u8'\0' );

	u = 'a';
	ASSERT_EQ( ConvertToUtf16( &u, 1, rftl::string_view( "\0", 2 ) ), 2 );
	ASSERT_CHAR( u, u'\0' );
	u = 'a';
	ASSERT_EQ( ConvertToUtf16( &u, 1, rftl::u8string_view( u8"\0", 2 ) ), 2 );
	ASSERT_CHAR( u, u'\0' );
	u = 'a';
	ASSERT_EQ( ConvertToUtf16( &u, 1, rftl::u16string_view( u"\0", 2 ) ), 2 );
	ASSERT_CHAR( u, u'\0' );
	u = 'a';
	ASSERT_EQ( ConvertToUtf16( &u, 1, rftl::u32string_view( U"\0", 2 ) ), 2 );
	ASSERT_CHAR( u, u'\0' );

	U = 'a';
	ASSERT_EQ( ConvertToUtf32( &U, 1, rftl::string_view( "\0", 2 ) ), 2 );
	ASSERT_CHAR( U, U'\0' );
	U = 'a';
	ASSERT_EQ( ConvertToUtf32( &U, 1, rftl::u8string_view( u8"\0", 2 ) ), 2 );
	ASSERT_CHAR( U, U'\0' );
	U = 'a';
	ASSERT_EQ( ConvertToUtf32( &U, 1, rftl::u16string_view( u"\0", 2 ) ), 2 );
	ASSERT_CHAR( U, U'\0' );
	U = 'a';
	ASSERT_EQ( ConvertToUtf32( &U, 1, rftl::u32string_view( U"\0", 2 ) ), 2 );
	ASSERT_CHAR( U, U'\0' );
}

///////////////////////////////////////////////////////////////////////////////
}
