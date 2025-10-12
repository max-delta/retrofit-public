#include "stdafx.h"

#include "rftl/extension/cstring_view.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( CStringView, BasicEmpty )
{
	rftl::cstring_view const empty;
	ASSERT_TRUE( empty.c_str() == nullptr );
	// The current implementation intentionally crashes this
	//ASSERT_TRUE( rftl::string_view( empty ).data() == nullptr );
	//ASSERT_TRUE( rftl::string_view( empty ).size() == 0 );
	ASSERT_TRUE( empty.force_view().data() == nullptr );
	ASSERT_TRUE( empty.force_view().size() == 0 );
}



TEST( CStringView, BasicPointer )
{
	char const* const str = "abc";
	rftl::cstring_view const view( str );
	ASSERT_TRUE( view.c_str() == str );
	ASSERT_TRUE( rftl::string_view( view ).data() == str );
	ASSERT_TRUE( rftl::string_view( view ).size() == 3 );
}



TEST( CStringView, BasicString )
{
	rftl::string const str = "abc";
	rftl::cstring_view const view( str );
	ASSERT_TRUE( view.c_str() == str.data() );
	ASSERT_TRUE( rftl::string_view( view ).data() == str.data() );
	ASSERT_TRUE( rftl::string_view( view ).size() == str.size() );
}

///////////////////////////////////////////////////////////////////////////////
}
