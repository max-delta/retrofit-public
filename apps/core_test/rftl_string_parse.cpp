#include "stdafx.h"

#include "rftl/extension/string_parse.h"

#include "rftl/array"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( StringParse, TrimPrefix )
{
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "" ), {} ) == "" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "" ), { 'a', 'z' } ) == "" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "##" ), { 'a', 'z' } ) == "##" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "a##" ), { 'a', 'z' } ) == "##" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "z##" ), { 'a', 'z' } ) == "##" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "#az##" ), { 'a', 'z' } ) == "#az##" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "##a" ), { 'a', 'z' } ) == "##a" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "##z" ), { 'a', 'z' } ) == "##z" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "#\0#" ), { 'a', 'z' } ) == "#\0#" );

	// Alternate form
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "#\0#" ), "" ) == "#\0#" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "#\0#" ), "az" ) == "#\0#" );
}



TEST( StringParse, TrimSuffix )
{
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "" ), {} ) == "" );
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "" ), { 'a', 'z' } ) == "" );
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "##" ), { 'a', 'z' } ) == "##" );
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "a##" ), { 'a', 'z' } ) == "a##" );
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "z##" ), { 'a', 'z' } ) == "z##" );
	static_assert( rftl::trim_prefix_chars( rftl::string_view( "#az##" ), { 'a', 'z' } ) == "#az##" );
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "##a" ), { 'a', 'z' } ) == "##" );
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "##z" ), { 'a', 'z' } ) == "##" );
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "#\0#" ), { 'a', 'z' } ) == "#\0#" );

	// Alternate form
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "#\0#" ), "" ) == "#\0#" );
	static_assert( rftl::trim_suffix_chars( rftl::string_view( "#\0#" ), "az" ) == "#\0#" );
}



TEST( StringParse, TryTrimSurrounds )
{
	static_assert( rftl::try_trim_surrounds( rftl::string_view( "" ), '{', '}' ) == "" );
	static_assert( rftl::try_trim_surrounds( rftl::string_view( "##" ), '{', '}' ) == "##" );
	static_assert( rftl::try_trim_surrounds( rftl::string_view( "{##" ), '{', '}' ) == "{##" );
	static_assert( rftl::try_trim_surrounds( rftl::string_view( "##}" ), '{', '}' ) == "##}" );
	static_assert( rftl::try_trim_surrounds( rftl::string_view( "{##}" ), '{', '}' ) == "##" );
	static_assert( rftl::try_trim_surrounds( rftl::string_view( "}##{" ), '{', '}' ) == "}##{" );
	static_assert( rftl::try_trim_surrounds( rftl::string_view( "+##+" ), '+', '+' ) == "##" );
	static_assert( rftl::try_trim_surrounds( rftl::string_view( "#\0#" ), '{', '}' ) == "#\0#" );
}



TEST( StringParse, TrimAfterFind )
{
	static_assert( rftl::trim_after_find_first_inclusive( rftl::string_view( "" ), 'a' ) == "" );
	static_assert( rftl::trim_after_find_first_inclusive( rftl::string_view( "##" ), 'a' ) == "##" );
	static_assert( rftl::trim_after_find_first_inclusive( rftl::string_view( "a##" ), 'a' ) == "" );
	static_assert( rftl::trim_after_find_first_inclusive( rftl::string_view( "#a#" ), 'a' ) == "#" );
	static_assert( rftl::trim_after_find_first_inclusive( rftl::string_view( "##a" ), 'a' ) == "##" );
}



TEST( StringParse, StrtokView )
{
	{
		// Empty
		rftl::string_view unparsed = "";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, '!' ), "" );
		ASSERT_EQ( unparsed, "" );
	}
	{
		// No token
		rftl::string_view unparsed = "012";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, '!' ), "012" );
		ASSERT_EQ( unparsed, "" );
	}
	{
		// Ends with token
		rftl::string_view unparsed = "012!";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, '!' ), "012" );
		ASSERT_EQ( unparsed, "" );
	}
	{
		// Ends with double token
		rftl::string_view unparsed = "012!!";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, '!' ), "012" );
		ASSERT_EQ( unparsed, "!" );
	}
	{
		// Split by token
		rftl::string_view unparsed = "012!345";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, '!' ), "012" );
		ASSERT_EQ( unparsed, "345" );
	}
	{
		// Split by double token
		rftl::string_view unparsed = "012!!345";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, '!' ), "012" );
		ASSERT_EQ( unparsed, "!345" );
	}
	{
		// Multiple tokens
		rftl::string_view unparsed = "012!345!678";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, '!' ), "012" );
		ASSERT_EQ( unparsed, "345!678" );
	}

	{
		// Alternate form
		rftl::string_view unparsed = "012!345!678";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, "!!" ), "012" );
		ASSERT_EQ( unparsed, "345!678" );
	}
	{
		// Alternate form
		rftl::string_view unparsed = "012!345!678";
		ASSERT_EQ( rftl::strtok_view<char>( unparsed, { '!', '!' } ), "012" );
		ASSERT_EQ( unparsed, "345!678" );
	}
}



TEST( StringParse, ParseInt )
{
#define RF_ASSERT__TRUE( STR, VAL ) \
	ASSERT_TRUE( rftl::parse_int( val, "" STR "" ) ); \
	ASSERT_EQ( val, VAL );
#define RF_ASSERT_FALSE( STR ) \
	ASSERT_FALSE( rftl::parse_int( val, "" STR "" ) ); \
	ASSERT_EQ( val, 0 );

	{
		uint8_t val = 0;
		RF_ASSERT_FALSE( "" );
		RF_ASSERT__TRUE( "0", 0 );
		RF_ASSERT__TRUE( "00000000", 0 );
		RF_ASSERT_FALSE( "0x0" );
		RF_ASSERT_FALSE( "0 " );
		RF_ASSERT_FALSE( " 0" );
		RF_ASSERT_FALSE( "-0" );
		RF_ASSERT_FALSE( "-1" );
		RF_ASSERT_FALSE( "-255" );
		RF_ASSERT_FALSE( "-256" );
		RF_ASSERT__TRUE( "255", 255 );
		RF_ASSERT_FALSE( "256" );
	}
	{
		int8_t val = 0;
		RF_ASSERT_FALSE( "" );
		RF_ASSERT__TRUE( "0", 0 );
		RF_ASSERT__TRUE( "00000000", 0 );
		RF_ASSERT_FALSE( "0x0" );
		RF_ASSERT_FALSE( "0 " );
		RF_ASSERT_FALSE( " 0" );
		RF_ASSERT__TRUE( "-0", 0 );
		RF_ASSERT__TRUE( "-1", -1 );
		RF_ASSERT__TRUE( "-128", -128 );
		RF_ASSERT_FALSE( "-129" );
		RF_ASSERT__TRUE( "127", 127 );
		RF_ASSERT_FALSE( "128" );
	}

#undef RF_ASSERT__TRUE
#undef RF_ASSERT_FALSE
}

///////////////////////////////////////////////////////////////////////////////
}
