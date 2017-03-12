#include "stdafx.h"
#include "core/macros.h"

// HACK
#include "core/ptr/ptr_ref.h"
#include "core/ptr/default_creator.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/weak_ptr.h"
#include <memory>

namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST(Bootstrap, Pass)
{
	RF_ASSERT(true);
	ASSERT_TRUE(true);
}



// HACK
TEST(Temp, QuickHacks)
{
	{
		std::shared_ptr<int> x ( new int(2) );
	}
	{
		UniquePtr<int> uptr( DefaultCreator<int>::Create(2) );

		WeakPtr<int> wptr( uptr );
		WeakPtr<int> wptr2( std::move(wptr) );
		uptr = nullptr;
		wptr2 = nullptr;
		wptr = nullptr;
	}
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
