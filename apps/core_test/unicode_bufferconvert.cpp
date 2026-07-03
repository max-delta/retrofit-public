#include "stdafx.h"

#include "core_unicode/BufferConvert.h"

#include "core_math/math_casts.h"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// GTest can't handle unicode
#define ASSERT_CHAR( A, B ) \
	ASSERT_EQ( ::RF::math::char_integer_bitcast( A ), ::RF::math::char_integer_bitcast( A ) )

template<typename CharT> static rftl::span<CharT, 0> NullSpan()
{
	return rftl::span<CharT, 0>();
}

template<typename CharT> static auto SingleSpan( CharT* ch ) -> rftl::span<CharT, 1>
{
	return rftl::span<CharT, 1>( ch, 1 );
}

}
///////////////////////////////////////////////////////////////////////////////

TEST( BufferConvert, EmptySource )
{
	ASSERT_EQ( ConvertToASCII( details::NullSpan<char>(), "" ), 0 );
	ASSERT_EQ( ConvertToASCII( details::NullSpan<char>(), u8"" ), 0 );
	ASSERT_EQ( ConvertToASCII( details::NullSpan<char>(), u"" ), 0 );
	ASSERT_EQ( ConvertToASCII( details::NullSpan<char>(), U"" ), 0 );

	ASSERT_EQ( ConvertToUtf8( details::NullSpan<char8_t>(), "" ), 0 );
	ASSERT_EQ( ConvertToUtf8( details::NullSpan<char8_t>(), u8"" ), 0 );
	ASSERT_EQ( ConvertToUtf8( details::NullSpan<char8_t>(), u"" ), 0 );
	ASSERT_EQ( ConvertToUtf8( details::NullSpan<char8_t>(), U"" ), 0 );

	ASSERT_EQ( ConvertToUtf16( details::NullSpan<char16_t>(), "" ), 0 );
	ASSERT_EQ( ConvertToUtf16( details::NullSpan<char16_t>(), u8"" ), 0 );
	ASSERT_EQ( ConvertToUtf16( details::NullSpan<char16_t>(), u"" ), 0 );
	ASSERT_EQ( ConvertToUtf16( details::NullSpan<char16_t>(), U"" ), 0 );

	ASSERT_EQ( ConvertToUtf32( details::NullSpan<char32_t>(), "" ), 0 );
	ASSERT_EQ( ConvertToUtf32( details::NullSpan<char32_t>(), u8"" ), 0 );
	ASSERT_EQ( ConvertToUtf32( details::NullSpan<char32_t>(), u"" ), 0 );
	ASSERT_EQ( ConvertToUtf32( details::NullSpan<char32_t>(), U"" ), 0 );
}



TEST( BufferConvert, EmptyDest )
{
	ASSERT_EQ( ConvertToASCII( details::NullSpan<char>(), "a" ), 1 );
	ASSERT_EQ( ConvertToASCII( details::NullSpan<char>(), u8"a" ), 1 );
	ASSERT_EQ( ConvertToASCII( details::NullSpan<char>(), u"a" ), 1 );
	ASSERT_EQ( ConvertToASCII( details::NullSpan<char>(), U"a" ), 1 );

	ASSERT_EQ( ConvertToUtf8( details::NullSpan<char8_t>(), "a" ), 1 );
	ASSERT_EQ( ConvertToUtf8( details::NullSpan<char8_t>(), u8"a" ), 1 );
	ASSERT_EQ( ConvertToUtf8( details::NullSpan<char8_t>(), u"a" ), 1 );
	ASSERT_EQ( ConvertToUtf8( details::NullSpan<char8_t>(), U"a" ), 1 );

	ASSERT_EQ( ConvertToUtf16( details::NullSpan<char16_t>(), "a" ), 1 );
	ASSERT_EQ( ConvertToUtf16( details::NullSpan<char16_t>(), u8"a" ), 1 );
	ASSERT_EQ( ConvertToUtf16( details::NullSpan<char16_t>(), u"a" ), 1 );
	ASSERT_EQ( ConvertToUtf16( details::NullSpan<char16_t>(), U"a" ), 1 );

	ASSERT_EQ( ConvertToUtf32( details::NullSpan<char32_t>(), "a" ), 1 );
	ASSERT_EQ( ConvertToUtf32( details::NullSpan<char32_t>(), u8"a" ), 1 );
	ASSERT_EQ( ConvertToUtf32( details::NullSpan<char32_t>(), u"a" ), 1 );
	ASSERT_EQ( ConvertToUtf32( details::NullSpan<char32_t>(), U"a" ), 1 );
}



TEST( BufferConvert, Null )
{
	char c_arr[2];
	char8_t u8_arr[2];
	char16_t u_arr[2];
	char32_t U_arr[2];

	char& c = c_arr[0];
	char8_t& u8 = u8_arr[0];
	char16_t& u = u_arr[0];
	char32_t& U = U_arr[0];

	char& Zc = c_arr[1];
	char8_t& Zu8 = u8_arr[1];
	char16_t& Zu = u_arr[1];
	char32_t& ZU = U_arr[1];

	// NOTE: Passing in strings with two null terminators, so the call will
	//  report that two writes were attempted, but only one will actually
	//  get applied
	Zc = '\1';
	Zu8 = u8'\1';
	Zu = u'\1';
	ZU = U'\1';

	c = 'a';
	ASSERT_EQ( ConvertToASCII( details::SingleSpan( &c ), rftl::string_view( "\0", 2 ) ), 2 );
	ASSERT_CHAR( c, '\0' );
	ASSERT_CHAR( Zc, '\1' );
	c = 'a';
	ASSERT_EQ( ConvertToASCII( details::SingleSpan( &c ), rftl::u8string_view( u8"\0", 2 ) ), 2 );
	ASSERT_CHAR( c, '\0' );
	ASSERT_CHAR( Zc, '\1' );
	c = 'a';
	ASSERT_EQ( ConvertToASCII( details::SingleSpan( &c ), rftl::u16string_view( u"\0", 2 ) ), 2 );
	ASSERT_CHAR( c, '\0' );
	ASSERT_CHAR( Zc, '\1' );
	c = 'a';
	ASSERT_EQ( ConvertToASCII( details::SingleSpan( &c ), rftl::u32string_view( U"\0", 2 ) ), 2 );
	ASSERT_CHAR( c, '\0' );
	ASSERT_CHAR( Zc, '\1' );

	u8 = 'a';
	ASSERT_EQ( ConvertToUtf8( details::SingleSpan( &u8 ), rftl::string_view( "\0", 2 ) ), 2 );
	ASSERT_CHAR( u8, u8'\0' );
	ASSERT_CHAR( Zu8, u8'\1' );
	u8 = 'a';
	ASSERT_EQ( ConvertToUtf8( details::SingleSpan( &u8 ), rftl::u8string_view( u8"\0", 2 ) ), 2 );
	ASSERT_CHAR( u8, u8'\0' );
	ASSERT_CHAR( Zu8, u8'\1' );
	u8 = 'a';
	ASSERT_EQ( ConvertToUtf8( details::SingleSpan( &u8 ), rftl::u16string_view( u"\0", 2 ) ), 2 );
	ASSERT_CHAR( u8, u8'\0' );
	ASSERT_CHAR( Zu8, u8'\1' );
	u8 = 'a';
	ASSERT_EQ( ConvertToUtf8( details::SingleSpan( &u8 ), rftl::u32string_view( U"\0", 2 ) ), 2 );
	ASSERT_CHAR( u8, u8'\0' );
	ASSERT_CHAR( Zu8, u8'\1' );

	u = 'a';
	ASSERT_EQ( ConvertToUtf16( details::SingleSpan( &u ), rftl::string_view( "\0", 2 ) ), 2 );
	ASSERT_CHAR( u, u'\0' );
	ASSERT_CHAR( Zu, u'\1' );
	u = 'a';
	ASSERT_EQ( ConvertToUtf16( details::SingleSpan( &u ), rftl::u8string_view( u8"\0", 2 ) ), 2 );
	ASSERT_CHAR( u, u'\0' );
	ASSERT_CHAR( Zu, u'\1' );
	u = 'a';
	ASSERT_EQ( ConvertToUtf16( details::SingleSpan( &u ), rftl::u16string_view( u"\0", 2 ) ), 2 );
	ASSERT_CHAR( u, u'\0' );
	ASSERT_CHAR( Zu, u'\1' );
	u = 'a';
	ASSERT_EQ( ConvertToUtf16( details::SingleSpan( &u ), rftl::u32string_view( U"\0", 2 ) ), 2 );
	ASSERT_CHAR( u, u'\0' );
	ASSERT_CHAR( Zu, u'\1' );

	U = 'a';
	ASSERT_EQ( ConvertToUtf32( details::SingleSpan( &U ), rftl::string_view( "\0", 2 ) ), 2 );
	ASSERT_CHAR( U, U'\0' );
	ASSERT_CHAR( ZU, U'\1' );
	U = 'a';
	ASSERT_EQ( ConvertToUtf32( details::SingleSpan( &U ), rftl::u8string_view( u8"\0", 2 ) ), 2 );
	ASSERT_CHAR( U, U'\0' );
	ASSERT_CHAR( ZU, U'\1' );
	U = 'a';
	ASSERT_EQ( ConvertToUtf32( details::SingleSpan( &U ), rftl::u16string_view( u"\0", 2 ) ), 2 );
	ASSERT_CHAR( U, U'\0' );
	ASSERT_CHAR( ZU, U'\1' );
	U = 'a';
	ASSERT_EQ( ConvertToUtf32( details::SingleSpan( &U ), rftl::u32string_view( U"\0", 2 ) ), 2 );
	ASSERT_CHAR( U, U'\0' );
	ASSERT_CHAR( ZU, U'\1' );
}

///////////////////////////////////////////////////////////////////////////////
}
