#include "stdafx.h"

#include "rftl/extension/variadic_print.h"
#include "rftl/array"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( VariadicPrint, FormatToN )
{
	rftl::array<char, 6> arr;
	rftl::span<char, 5> span( arr.data(), 5 );
	rftl::span<char> result;

	arr.fill( 1 );

	result = rftl::var_format_to_n( span, "{}", 23 );
	ASSERT_TRUE( result.data() == arr.data() );
	ASSERT_TRUE( result.size() == 3 );
	ASSERT_TRUE( arr.at( 0 ) == '2' );
	ASSERT_TRUE( arr.at( 1 ) == '3' );
	ASSERT_TRUE( arr.at( 2 ) == '\0' ); // True null
	ASSERT_TRUE( arr.at( 3 ) == 1 );
	ASSERT_TRUE( arr.at( 4 ) == '\0' ); // Paranoid null
	ASSERT_TRUE( arr.at( 5 ) == 1 );

	arr.fill( 1 );

	result = rftl::var_format_to_n( span, "{}", 23456 );
	ASSERT_TRUE( result.data() == arr.data() );
	ASSERT_TRUE( result.size() == 5 );
	ASSERT_TRUE( arr.at( 0 ) == '2' );
	ASSERT_TRUE( arr.at( 1 ) == '3' );
	ASSERT_TRUE( arr.at( 2 ) == '4' );
	ASSERT_TRUE( arr.at( 3 ) == '5' );
	ASSERT_TRUE( arr.at( 4 ) == '\0' ); // Truncate null
	ASSERT_TRUE( arr.at( 5 ) == 1 );
}



TEST( VariadicPrint, VFormatTo )
{
	rftl::array<char, 6> arr;
	rftl::span<char, 5> span( arr.data(), 5 );
	uint16_t in;
	rftl::span<char> result;

	arr.fill( 1 );

	in = 23;
	result = rftl::var_vformat_to( span, "{}", rftl::make_format_args( in ) );
	ASSERT_TRUE( result.data() == arr.data() );
	ASSERT_TRUE( result.size() == 3 );
	ASSERT_TRUE( arr.at( 0 ) == '2' );
	ASSERT_TRUE( arr.at( 1 ) == '3' );
	ASSERT_TRUE( arr.at( 2 ) == '\0' ); // True null
	ASSERT_TRUE( arr.at( 3 ) == 1 );
	ASSERT_TRUE( arr.at( 4 ) == '\0' ); // Paranoid null
	ASSERT_TRUE( arr.at( 5 ) == 1 );

	arr.fill( 1 );

	in = 23456;
	result = rftl::var_vformat_to( span, "{}", rftl::make_format_args( in ) );
	ASSERT_TRUE( result.data() == arr.data() );
	ASSERT_TRUE( result.size() == 5 );
	ASSERT_TRUE( arr.at( 0 ) == '2' );
	ASSERT_TRUE( arr.at( 1 ) == '3' );
	ASSERT_TRUE( arr.at( 2 ) == '4' );
	ASSERT_TRUE( arr.at( 3 ) == '5' );
	ASSERT_TRUE( arr.at( 4 ) == '\0' ); // Truncate null
	ASSERT_TRUE( arr.at( 5 ) == 1 );
}

///////////////////////////////////////////////////////////////////////////////
}
