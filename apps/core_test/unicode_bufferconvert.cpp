#include "stdafx.h"

#include "core_unicode/BufferConvert.h"


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

TEST( BufferConvert, EmptySource )
{
	ASSERT_EQ( ConvertToASCII( nullptr, 0, "", 0 ), 0 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, u"", 0 ), 0 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, U"", 0 ), 0 );

	ASSERT_EQ( ConvertToUtf8( nullptr, 0, "", 0 ), 0 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, u"", 0 ), 0 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, U"", 0 ), 0 );

	ASSERT_EQ( ConvertToUtf16( nullptr, 0, "", 0 ), 0 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, u"", 0 ), 0 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, U"", 0 ), 0 );

	ASSERT_EQ( ConvertToUtf32( nullptr, 0, "", 0 ), 0 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, u"", 0 ), 0 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, U"", 0 ), 0 );
}



TEST( BufferConvert, EmptyDest )
{
	ASSERT_EQ( ConvertToASCII( nullptr, 0, "a", 1 ), 1 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, u"a", 1 ), 1 );
	ASSERT_EQ( ConvertToASCII( nullptr, 0, U"a", 1 ), 1 );

	ASSERT_EQ( ConvertToUtf8( nullptr, 0, "a", 1 ), 1 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, u"a", 1 ), 1 );
	ASSERT_EQ( ConvertToUtf8( nullptr, 0, U"a", 1 ), 1 );

	ASSERT_EQ( ConvertToUtf16( nullptr, 0, "a", 1 ), 1 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, u"a", 1 ), 1 );
	ASSERT_EQ( ConvertToUtf16( nullptr, 0, U"a", 1 ), 1 );

	ASSERT_EQ( ConvertToUtf32( nullptr, 0, "a", 1 ), 1 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, u"a", 1 ), 1 );
	ASSERT_EQ( ConvertToUtf32( nullptr, 0, U"a", 1 ), 1 );
}



TEST( BufferConvert, Null )
{
	char c;
	char16_t u;
	char32_t U;

	c = 'a';
	ASSERT_EQ( ConvertToASCII( &c, 1, "\0", 2 ), 2 );
	ASSERT_EQ( c, '\0' );
	c = 'a';
	ASSERT_EQ( ConvertToASCII( &c, 1, u"\0", 2 ), 2 );
	ASSERT_EQ( c, '\0' );
	c = 'a';
	ASSERT_EQ( ConvertToASCII( &c, 1, U"\0", 2 ), 2 );
	ASSERT_EQ( c, '\0' );

	c = 'a';
	ASSERT_EQ( ConvertToUtf8( &c, 1, "\0", 2 ), 2 );
	ASSERT_EQ( c, '\0' );
	c = 'a';
	ASSERT_EQ( ConvertToUtf8( &c, 1, u"\0", 2 ), 2 );
	ASSERT_EQ( c, '\0' );
	c = 'a';
	ASSERT_EQ( ConvertToUtf8( &c, 1, U"\0", 2 ), 2 );
	ASSERT_EQ( c, '\0' );

	u = 'a';
	ASSERT_EQ( ConvertToUtf16( &u, 1, "\0", 2 ), 2 );
	ASSERT_EQ( u, u'\0' );
	u = 'a';
	ASSERT_EQ( ConvertToUtf16( &u, 1, u"\0", 2 ), 2 );
	ASSERT_EQ( u, u'\0' );
	u = 'a';
	ASSERT_EQ( ConvertToUtf16( &u, 1, U"\0", 2 ), 2 );
	ASSERT_EQ( u, u'\0' );

	U = 'a';
	ASSERT_EQ( ConvertToUtf32( &U, 1, "\0", 2 ), 2 );
	ASSERT_EQ( U, U'\0' );
	U = 'a';
	ASSERT_EQ( ConvertToUtf32( &U, 1, u"\0", 2 ), 2 );
	ASSERT_EQ( U, U'\0' );
	U = 'a';
	ASSERT_EQ( ConvertToUtf32( &U, 1, U"\0", 2 ), 2 );
	ASSERT_EQ( U, U'\0' );
}

///////////////////////////////////////////////////////////////////////////////
}}
