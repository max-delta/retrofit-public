#include "stdafx.h"

#include "rftl/extension/string_traits.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( StringTraits, IsChar )
{
	static_assert( rftl::is_char<char>::value, "Unexpected value" );
	static_assert( rftl::is_char<wchar_t>::value, "Unexpected value" );
	static_assert( rftl::is_char<char8_t>::value, "Unexpected value" );
	static_assert( rftl::is_char<char16_t>::value, "Unexpected value" );
	static_assert( rftl::is_char<char32_t>::value, "Unexpected value" );

	static_assert( rftl::is_char<char const>::value, "Unexpected value" );
	static_assert( rftl::is_char<wchar_t const>::value, "Unexpected value" );
	static_assert( rftl::is_char<char8_t const>::value, "Unexpected value" );
	static_assert( rftl::is_char<char16_t const>::value, "Unexpected value" );
	static_assert( rftl::is_char<char32_t const>::value, "Unexpected value" );

	static_assert( rftl::is_char<decltype( '\0' )>::value, "Unexpected value" );
	static_assert( rftl::is_char<decltype( L'\0' )>::value, "Unexpected value" );
	static_assert( rftl::is_char<decltype( u8'\0' )>::value, "Unexpected value" );
	static_assert( rftl::is_char<decltype( u'\0' )>::value, "Unexpected value" );
	static_assert( rftl::is_char<decltype( U'\0' )>::value, "Unexpected value" );

	static_assert( rftl::is_char<decltype( "\0"[0] )>::value, "Unexpected value" );
	static_assert( rftl::is_char<decltype( L"\0"[0] )>::value, "Unexpected value" );
	static_assert( rftl::is_char<decltype( u8"\0"[0] )>::value, "Unexpected value" );
	static_assert( rftl::is_char<decltype( u"\0"[0] )>::value, "Unexpected value" );
	static_assert( rftl::is_char<decltype( U"\0"[0] )>::value, "Unexpected value" );

	static_assert( rftl::is_char<int8_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_char<int16_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_char<int32_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_char<int64_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_char<int>::value == false, "Unexpected value" );
	static_assert( rftl::is_char<short>::value == false, "Unexpected value" );
	static_assert( rftl::is_char<long>::value == false, "Unexpected value" );

	static_assert( rftl::is_char<decltype( 0 )>::value == false, "Unexpected value" );
	static_assert( rftl::is_char<decltype( 0l )>::value == false, "Unexpected value" );
	static_assert( rftl::is_char<decltype( 0ll )>::value == false, "Unexpected value" );
}



TEST( StringTraits, IsCharArray )
{
	static_assert( rftl::is_char_array<char[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t[]>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<char const[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t const[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t const[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t const[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t const[]>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<char( & )[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t( & )[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t( & )[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t( & )[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t( & )[]>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<char const( & )[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t const( & )[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t const( & )[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t const( & )[]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t const( & )[]>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<char[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t[1]>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<char const[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t const[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t const[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t const[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t const[1]>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<char( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t( & )[1]>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<char const( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t const( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t const( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t const( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t const( & )[1]>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<decltype( "" )>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<decltype( L"" )>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<decltype( u"" )>::value, "Unexpected value" );
	static_assert( rftl::is_char_array<decltype( U"" )>::value, "Unexpected value" );

	static_assert( rftl::is_char_array<char>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t>::value == false, "Unexpected value" );

	static_assert( rftl::is_char_array<char const>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t const>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t const>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t const>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t const>::value == false, "Unexpected value" );

	static_assert( rftl::is_char_array<char*>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t*>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t*>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t*>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t*>::value == false, "Unexpected value" );

	static_assert( rftl::is_char_array<char const*>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<wchar_t const*>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char8_t const*>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char16_t const*>::value == false, "Unexpected value" );
	static_assert( rftl::is_char_array<char32_t const*>::value == false, "Unexpected value" );
}



TEST( StringTraits, IsSizedCharArray )
{
	static_assert( rftl::is_sized_char_array<char[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t[]>::value == false, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char const[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t const[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t const[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t const[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t const[]>::value == false, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char( & )[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t( & )[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t( & )[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t( & )[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t( & )[]>::value == false, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char const( & )[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t const( & )[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t const( & )[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t const( & )[]>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t const( & )[]>::value == false, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t[1]>::value, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char const[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t const[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t const[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t const[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t const[1]>::value, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t( & )[1]>::value, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char const( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t const( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t const( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t const( & )[1]>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t const( & )[1]>::value, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<decltype( "" )>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<decltype( L"" )>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<decltype( u8"" )>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<decltype( u"" )>::value, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<decltype( U"" )>::value, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t>::value == false, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char const>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t const>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t const>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t const>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t const>::value == false, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char*>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t*>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t*>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t*>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t*>::value == false, "Unexpected value" );

	static_assert( rftl::is_sized_char_array<char const*>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<wchar_t const*>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char8_t const*>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char16_t const*>::value == false, "Unexpected value" );
	static_assert( rftl::is_sized_char_array<char32_t const*>::value == false, "Unexpected value" );
}

///////////////////////////////////////////////////////////////////////////////
}
