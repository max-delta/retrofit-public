#include "stdafx.h"

#include "rftl/extension/bounded_overwrite_iterator.h"

#include "rftl/array"
#include "rftl/span"
#include "rftl/string_view"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( BoundedOverwriteIterator, BasicOperations )
{
	rftl::array<char, 4> buf = { 'a', 'b', 'c', 'd' };
	rftl::string_view const view( buf.data(), buf.size() );
	rftl::span const narrow = rftl::span( buf ).subspan( 0, 3 );

	rftl::bounded_forward_overwrite_iterator iter( narrow );

	ASSERT_TRUE( iter.tell() == narrow.begin() + 0 );
	ASSERT_TRUE( view == "abcd" );
	*iter = '1';
	ASSERT_TRUE( iter.tell() == narrow.begin() + 1 );
	ASSERT_TRUE( view == "1bcd" );
	*iter = '2';
	ASSERT_TRUE( iter.tell() == narrow.begin() + 2 );
	ASSERT_TRUE( view == "12cd" );
	*iter = '3';
	ASSERT_TRUE( iter.tell() == narrow.begin() + 3 );
	ASSERT_TRUE( view == "123d" );
	*iter = '4';
	ASSERT_TRUE( iter.tell() == narrow.begin() + 3 );
	ASSERT_TRUE( view == "123d" );
}

///////////////////////////////////////////////////////////////////////////////
}
