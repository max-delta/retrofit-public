#include "stdafx.h"

#include "rftl/array"
#include "rftl/extension/string_format.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( StringFormat, FormatNibble )
{
	for( uint8_t i = 0x0; i <= 0x9; i++ )
	{
		char const ch = rftl::format_nibble( i );
		ASSERT_EQ( ch, '0' + i );
	}
	for( uint8_t i = 0xA; i <= 0xF; i++ )
	{
		char const ch = rftl::format_nibble( i );
		ASSERT_EQ( ch, 'A' + i - 0xA );
	}
	for( uint8_t i = 0x10; i < 0xFF; i++ )
	{
		char const ch = rftl::format_nibble( i );
		ASSERT_EQ( ch, '#' );
	}
	ASSERT_EQ( rftl::format_nibble( 0xFF ), '#' );
}



TEST( StringFormat, ToString )
{
	rftl::array<uint8_t, 18> const data =
		{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 255 };
	rftl::byte_view const view{ data.data(), data.size() };

	rftl::string const full = rftl::to_string( view, 20 );
	ASSERT_EQ( full, "0102030405060708090A0B0C0D0E0F1011FF" );

	rftl::string const truncated = rftl::to_string( view, 5 );
	ASSERT_EQ( truncated, "0102030405..." );
}

///////////////////////////////////////////////////////////////////////////////
}
