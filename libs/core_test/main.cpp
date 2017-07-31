#include "stdafx.h"
#include "core/macros.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST(Bootstrap, Pass)
{
	RF_ASSERT(true);
	ASSERT_TRUE(true);
}

///////////////////////////////////////////////////////////////////////////////
}

int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int retVal = RUN_ALL_TESTS();
	if( retVal != 0)
	{
		system("pause");
	}
	return retVal;
}
