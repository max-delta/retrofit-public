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



// HACK
// TODO: Move to seperate file and cleanup
TEST(Temp, QuickHacks)
{
	{
		std::shared_ptr<int> x ( new int(2) );
		int* q = x.get();
	}
	{
		UniquePtr<int> uptr( DefaultCreator<int>::Create(2) );
		ASSERT_TRUE(uptr != nullptr && *uptr == 2);
		UniquePtr<int> uptr2( std::move(uptr) );
		ASSERT_TRUE(uptr == nullptr);
		ASSERT_TRUE(uptr2 != nullptr && *uptr2 == 2);
		uptr = std::move(uptr2);
		ASSERT_TRUE(uptr != nullptr && *uptr == 2);
		ASSERT_TRUE(uptr2 == nullptr);

		WeakPtr<int> wptr( uptr );
		WeakPtr<int> wptr2( std::move(wptr) );
		uptr = nullptr;
		wptr2 = nullptr;
		wptr = nullptr;
	}
	{
		SharedPtr<int> sptr( DefaultCreator<int>::Create(2) );
		ASSERT_TRUE(sptr != nullptr && *sptr == 2);
		SharedPtr<int> sptr2( std::move(sptr) );
		ASSERT_TRUE(sptr == nullptr);
		ASSERT_TRUE(sptr2 != nullptr && *sptr2 == 2);
		sptr = std::move(sptr2);
		ASSERT_TRUE(sptr != nullptr && *sptr == 2);
		ASSERT_TRUE(sptr2 == nullptr);
		SharedPtr<int> sptr3( sptr );
		ASSERT_TRUE(sptr != nullptr && *sptr == 2);
		ASSERT_TRUE(sptr2 == nullptr);
		ASSERT_TRUE(sptr3 != nullptr && *sptr3 == 2);
		sptr3 = nullptr;
		ASSERT_TRUE(sptr != nullptr && *sptr == 2);
		ASSERT_TRUE(sptr2 == nullptr);
		ASSERT_TRUE(sptr3 == nullptr);
		sptr3 = sptr;
		ASSERT_TRUE(sptr != nullptr && *sptr == 2);
		ASSERT_TRUE(sptr2 == nullptr);
		ASSERT_TRUE(sptr3 != nullptr && *sptr3 == 2);

		{
			WeakSharedPtr<int> wsptr = sptr;
			wsptr = nullptr;
			wsptr = sptr;
			{
				SharedPtr<int> temps = wsptr.Lock();
			}
			{
				WeakPtr<int> temp = wsptr;
			}
		}

		WeakPtr<int> wptr( sptr );
		WeakPtr<int> wptr2( wptr );
		wptr2 = nullptr;
		wptr2 = wptr;
		sptr = nullptr;
		wptr = nullptr;
		sptr3 = nullptr;
		wptr2 = nullptr;
	}
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
