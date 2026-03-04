#include "stdafx.h"

#include "rftl/extension/unique_char_set.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( UniqueCharSet, BasicEmpty )
{
	rftl::unique_char_set const empty;
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.count( '@' ) == 0 );
	ASSERT_TRUE( empty.count( '\0' ) == 0 );
}



TEST( UniqueCharSet, FromInitalizer )
{
	ASSERT_TRUE( rftl::unique_char_set( { 'c', 'a', 'b' } ) == "abc" );
	ASSERT_TRUE( rftl::unique_char_set( { 'c', 'a', 'b', 'c', 'a', 'b' } ) == "abc" );
}



TEST( UniqueCharSet, FromLiteral )
{
	ASSERT_TRUE( rftl::unique_char_set( "cab" ) == "abc" );
	ASSERT_TRUE( rftl::unique_char_set( "cabcab" ) == "abc" );
}



TEST( UniqueCharSet, Lookup )
{
	rftl::unique_char_set const str( "cab" );
	ASSERT_TRUE( str.count( '@' ) == 0 );
	ASSERT_TRUE( str.count( 'b' ) == 1 );
	ASSERT_TRUE( str.count( '\0' ) == 0 );

	rftl::unique_char_set const null( { '\0' } );
	ASSERT_TRUE( null.count( '\0' ) == 1 );
}

///////////////////////////////////////////////////////////////////////////////
}
