#include "stdafx.h"

#include "core/ptr/shared_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

namespace{
	struct HugeStruct
	{
		uint64_t member[100];
	};
}
static_assert(
	sizeof(SharedPtr<int>) < sizeof(HugeStruct),
	"SharedPtr is embedding storage" );



TEST(SharedPtr, Defaults)
{
	{
		SharedPtr<int> sptr;
		ASSERT_TRUE(sptr == nullptr);
	}
	{
		SharedPtr<int> sptr = SharedPtr<int>();
		ASSERT_TRUE(sptr == nullptr);
	}
	{
		SharedPtr<int> sptr = nullptr;
		ASSERT_TRUE(sptr == nullptr);
	}
}



TEST(SharedPtr, SoloLifecycle)
{
	// Normal
	{
		SharedPtr<int> sptr = DefaultCreator<int>::Create(47);
		ASSERT_TRUE(sptr != nullptr);
		ASSERT_TRUE(*sptr == 47);
		*sptr = 53;
		ASSERT_TRUE(*sptr == 53);
	}

	// Deferred create
	{
		auto creation = DefaultCreator<int>::Create(47);
		{
			SharedPtr<int> sptr = std::move(creation);
			ASSERT_TRUE(sptr != nullptr);
			ASSERT_TRUE(*sptr == 47);
			*sptr = 53;
			ASSERT_TRUE(*sptr == 53);
		}
	}

	// Pre-mature termination
	{
		SharedPtr<int> sptr = DefaultCreator<int>::Create(47);
		ASSERT_TRUE(sptr != nullptr);
		sptr = nullptr;
		ASSERT_TRUE(sptr == nullptr);
	}

}



TEST(SharedPtr, WeakLifecycle)
{
	// Weak outlives
	{
		WeakPtr<int> wptr;
		ASSERT_TRUE(wptr == nullptr);
		{
			SharedPtr<int> sptr = DefaultCreator<int>::Create(47);
			wptr = sptr;
			ASSERT_TRUE(wptr != nullptr);
		}
		ASSERT_TRUE(wptr == nullptr);
	}

	// Shared outlives
	{
		SharedPtr<int> sptr = DefaultCreator<int>::Create(47);
		ASSERT_TRUE(sptr != nullptr);
		{
			WeakPtr<int> wptr = sptr;
			ASSERT_TRUE(wptr != nullptr);
		}
		ASSERT_TRUE(sptr != nullptr);
	}
}



TEST(SharedPtr, Move)
{
	// Move to longer-lived shared
	{
		SharedPtr<int> sptr2;
		ASSERT_TRUE(sptr2 == nullptr);
		{
			SharedPtr<int> sptr1 = DefaultCreator<int>::Create(47);
			ASSERT_TRUE(sptr1 != nullptr);
			ASSERT_TRUE(*sptr1 == 47);
			sptr2 = std::move(sptr1);
			ASSERT_TRUE(sptr1 == nullptr);
			ASSERT_TRUE(sptr2 != nullptr);
			ASSERT_TRUE(*sptr2 == 47);
		}
		ASSERT_TRUE(sptr2 != nullptr);
		ASSERT_TRUE(*sptr2 == 47);
	}
}



TEST(SharedPtr, Copy)
{
	// Copy to longer-lived shared
	{
		SharedPtr<int> sptr2;
		ASSERT_TRUE(sptr2 == nullptr);
		{
			SharedPtr<int> sptr1 = DefaultCreator<int>::Create(47);
			ASSERT_TRUE(sptr1 != nullptr);
			ASSERT_TRUE(*sptr1 == 47);
			sptr2 = sptr1;
			ASSERT_TRUE(sptr1 != nullptr);
			ASSERT_TRUE(*sptr1 == 47);
			ASSERT_TRUE(sptr2 != nullptr);
			ASSERT_TRUE(*sptr2 == 47);
		}
		ASSERT_TRUE(sptr2 != nullptr);
		ASSERT_TRUE(*sptr2 == 47);
	}
}



TEST(SharedPtr, Arrow)
{
	struct Test
	{
		int test;
	};

	SharedPtr<Test> sptr = DefaultCreator<Test>::Create();
	WeakPtr<Test> wptr = sptr;
	( *sptr ).test = 9;
	ASSERT_TRUE( ( *sptr ).test == 9 );
	ASSERT_TRUE( ( *wptr ).test == 9 );
	ASSERT_TRUE( sptr->test == 9 );
	ASSERT_TRUE( wptr->test == 9 );
}

///////////////////////////////////////////////////////////////////////////////
}
