#include "stdafx.h"

#include "core/ptr/unique_ptr.h"
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
	sizeof(UniquePtr<int>) < sizeof(HugeStruct),
	"UniquePtr is embedding storage" );



TEST(UniquePtr, Defaults)
{
	{
		UniquePtr<int> uptr;
		ASSERT_TRUE(uptr == nullptr);
	}
	{
		UniquePtr<int> uptr = UniquePtr<int>();
		ASSERT_TRUE(uptr == nullptr);
	}
	{
		UniquePtr<int> uptr = nullptr;
		ASSERT_TRUE(uptr == nullptr);
	}
}



TEST(UniquePtr, SoloLifecycle)
{
	// Normal
	{
		UniquePtr<int> uptr = DefaultCreator<int>::Create(47);
		ASSERT_TRUE(uptr != nullptr);
		ASSERT_TRUE(*uptr == 47);
		*uptr = 53;
		ASSERT_TRUE(*uptr == 53);
	}

	// Deferred create
	{
		auto creation = DefaultCreator<int>::Create(47);
		{
			UniquePtr<int> uptr = std::move(creation);
			ASSERT_TRUE(uptr != nullptr);
			ASSERT_TRUE(*uptr == 47);
			*uptr = 53;
			ASSERT_TRUE(*uptr == 53);
		}
	}

	// Pre-mature termination
	{
		UniquePtr<int> uptr = DefaultCreator<int>::Create(47);
		ASSERT_TRUE(uptr != nullptr);
		uptr = nullptr;
		ASSERT_TRUE(uptr == nullptr);
	}

}



TEST(UniquePtr, WeakLifecycle)
{
	// Weak outlives
	{
		WeakPtr<int> wptr;
		ASSERT_TRUE(wptr == nullptr);
		{
			UniquePtr<int> uptr = DefaultCreator<int>::Create(47);
			wptr = uptr;
			ASSERT_TRUE(wptr != nullptr);
		}
		ASSERT_TRUE(wptr == nullptr);
	}

	// Unique outlives
	{
		UniquePtr<int> uptr = DefaultCreator<int>::Create(47);
		ASSERT_TRUE(uptr != nullptr);
		{
			WeakPtr<int> wptr = uptr;
			ASSERT_TRUE(wptr != nullptr);
		}
		ASSERT_TRUE(uptr != nullptr);
	}
}



TEST(UniquePtr, Move)
{
	// Move to longer-lived unique
	{
		UniquePtr<int> uptr2;
		ASSERT_TRUE(uptr2 == nullptr);
		{
			UniquePtr<int> uptr1 = DefaultCreator<int>::Create(47);
			ASSERT_TRUE(uptr1 != nullptr);
			ASSERT_TRUE(*uptr1 == 47);
			uptr2 = std::move(uptr1);
			ASSERT_TRUE(uptr1 == nullptr);
			ASSERT_TRUE(uptr2 != nullptr);
			ASSERT_TRUE(*uptr2 == 47);
		}
		ASSERT_TRUE(uptr2 != nullptr);
		ASSERT_TRUE(*uptr2 == 47);
	}
}



TEST(UniquePtr, Arrow)
{
	struct Test
	{
		int test;
	};

	UniquePtr<Test> uptr = DefaultCreator<Test>::Create();
	( *uptr ).test = 9;
	ASSERT_TRUE( ( *uptr ).test == 9 );
	ASSERT_TRUE( uptr->test == 9 );
}

///////////////////////////////////////////////////////////////////////////////
}
