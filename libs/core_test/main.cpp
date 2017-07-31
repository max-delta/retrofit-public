#include "stdafx.h"
#include "core/macros.h"

// HACK
// TODO: Move to seperate file and cleanup
#include "core/ptr/ptr_ref.h"
#include "core/ptr/default_creator.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/shared_ptr.h"
#include "core/ptr/weak_ptr.h"
#include "core/ptr/weak_shared_ptr.h"
#include <memory>


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
