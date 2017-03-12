#include "stdafx.h"

TEST(Bootstrap, Pass)
{
	//
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
