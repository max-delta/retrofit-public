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

///////////////////////////////////////////////////////////////////////////////
}
