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



TEST( StringFormat, FormatByte )
{
	char ch1, ch2 = 0;

	rftl::format_byte( ch1, ch2, 0x00 );
	ASSERT_EQ( ch1, '0' );
	ASSERT_EQ( ch2, '0' );

	rftl::format_byte( ch1, ch2, 0x12 );
	ASSERT_EQ( ch1, '1' );
	ASSERT_EQ( ch2, '2' );

	rftl::format_byte( ch1, ch2, 0x3C );
	ASSERT_EQ( ch1, '3' );
	ASSERT_EQ( ch2, 'C' );

	rftl::format_byte( ch1, ch2, 0xDE );
	ASSERT_EQ( ch1, 'D' );
	ASSERT_EQ( ch2, 'E' );

	rftl::format_byte( ch1, ch2, 0xFF );
	ASSERT_EQ( ch1, 'F' );
	ASSERT_EQ( ch2, 'F' );
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
