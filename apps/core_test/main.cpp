#include "stdafx.h"
#include "core/macros.h"

#include "rftl/string"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( Bootstrap, Pass )
{
	RF_ASSERT( true );
	ASSERT_TRUE( true );
}

///////////////////////////////////////////////////////////////////////////////
}

int main( int argc, char** argv )
{
	::testing::InitGoogleTest( &argc, argv );
	int const retVal = RUN_ALL_TESTS();
	if( retVal != 0 )
	{
		if( argc >= 2 && rftl::string( argv[1] ) == "--nopause" )
		{
			fputs( "One or more tests failed", stderr );
		}
		else
		{
			system( "pause" );
		}
	}
	return retVal;
}
