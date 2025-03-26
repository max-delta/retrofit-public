#include "stdafx.h"

#include "rftl/array"
#include "rftl/extension/string_format.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

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
