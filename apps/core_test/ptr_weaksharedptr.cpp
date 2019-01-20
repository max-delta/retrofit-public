#include "stdafx.h"

#include "core/ptr/weak_shared_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

namespace {

struct HugeStruct
{
	uint64_t member[100];
};

}
static_assert(
	sizeof( WeakSharedPtr<HugeStruct> ) < sizeof( HugeStruct ),
	"WeakSharedPtr is embedding storage" );



TEST( WeakSharedPtr, Defaults )
{
	{
		WeakSharedPtr<int> wsptr;
		ASSERT_TRUE( wsptr.Weaken() == nullptr );
	}
	{
		WeakSharedPtr<int> wsptr = WeakSharedPtr<int>();
		ASSERT_TRUE( wsptr.Weaken() == nullptr );
	}
	{
		WeakSharedPtr<int> wsptr = nullptr;
		ASSERT_TRUE( wsptr.Weaken() == nullptr );
	}
}



TEST( WeakSharedPtr, MeaninglessChurn )
{
	// Implicit cast into a stronger pointer, which immediately drops out of
	//  it's lifecylce, leaving the WeakSharedPtr holding onto a dead pointer
	{
		WeakSharedPtr<int> wsptr = WeakSharedPtr<int>( DefaultCreator<int>::Create( 47 ) );
		ASSERT_TRUE( wsptr.Weaken() == nullptr );
		ASSERT_TRUE( wsptr.Lock() == nullptr );
	}
}



TEST( WeakSharedPtr, SoloLifecycle )
{
	// Weak
	{
		SharedPtr<int> source = DefaultCreator<int>::Create( 47 );
		{
			WeakSharedPtr<int> wsptr = source;
			ASSERT_TRUE( wsptr.Weaken() != nullptr );
			ASSERT_TRUE( *wsptr.Weaken() == 47 );
			*wsptr.Weaken() = 53;
			ASSERT_TRUE( *wsptr.Weaken() == 53 );
		}
	}

	// Strong
	{
		SharedPtr<int> source = DefaultCreator<int>::Create( 47 );
		{
			WeakSharedPtr<int> wsptr = source;
			ASSERT_TRUE( wsptr.Weaken() != nullptr );
			ASSERT_TRUE( *wsptr.Weaken() == 47 );
			*wsptr.Lock() = 53;
			ASSERT_TRUE( *wsptr.Weaken() == 53 );
		}
	}

	// Pre-mature termination
	{
		SharedPtr<int> source = DefaultCreator<int>::Create( 47 );
		{
			WeakSharedPtr<int> wsptr = source;
			ASSERT_TRUE( wsptr.Weaken() != nullptr );
			wsptr = nullptr;
			ASSERT_TRUE( wsptr.Weaken() == nullptr );
		}
	}
}



TEST( WeakSharedPtr, Cyclic )
{
	// Initialize ourselves with a strengthened pointer we made
	{
		SharedPtr<int> source = DefaultCreator<int>::Create( 47 );
		{
			WeakSharedPtr<int> wsptr = source;
			ASSERT_TRUE( wsptr.Weaken() != nullptr );
			ASSERT_TRUE( *wsptr.Weaken() == 47 );
			{
				SharedPtr<int> sptr = wsptr.Lock();
				ASSERT_TRUE( wsptr.Weaken() != nullptr );
				ASSERT_TRUE( *wsptr.Weaken() == 47 );
				ASSERT_TRUE( sptr != nullptr );
				ASSERT_TRUE( *sptr == 47 );
				wsptr = sptr;
				ASSERT_TRUE( wsptr.Weaken() != nullptr );
				ASSERT_TRUE( *wsptr.Weaken() == 47 );
				ASSERT_TRUE( sptr != nullptr );
				ASSERT_TRUE( *sptr == 47 );
			}
			ASSERT_TRUE( wsptr.Weaken() != nullptr );
			ASSERT_TRUE( *wsptr.Weaken() == 47 );
		}
	}
}



TEST( WeakSharedPtr, Move )
{
	// Move to longer-lived pointer
	{
		SharedPtr<int> source = DefaultCreator<int>::Create( 47 );
		{
			WeakSharedPtr<int> wsptr2;
			ASSERT_TRUE( wsptr2.Weaken() == nullptr );
			{
				WeakSharedPtr<int> wsptr1 = source;
				ASSERT_TRUE( wsptr1.Weaken() != nullptr );
				ASSERT_TRUE( *wsptr1.Weaken() == 47 );
				wsptr2 = rftl::move( wsptr1 );
				ASSERT_TRUE( wsptr1.Weaken() == nullptr );
				ASSERT_TRUE( wsptr2.Weaken() != nullptr );
				ASSERT_TRUE( *wsptr2.Weaken() == 47 );
			}
			ASSERT_TRUE( wsptr2.Weaken() != nullptr );
			ASSERT_TRUE( *wsptr2.Weaken() == 47 );
		}
	}
}



// Not currently supported. Is there a reason to?
//TEST( WeakSharedPtr, MoveDerived )
//{
//	struct Base
//	{
//		virtual ~Base() = default;
//		int base;
//	};
//	struct Derived : public Base
//	{
//		int derived;
//	};
//
//	// Construct into move
//	{
//		WeakSharedPtr<Derived> wsptr1 = DefaultCreator<Derived>::Create();
//		ASSERT_TRUE(wsptr1 != nullptr);
//		WeakSharedPtr<Base> uptr2( rftl::move( wsptr1 ) );
//		ASSERT_TRUE(wsptr1 == nullptr);
//		ASSERT_TRUE(wsptr2 != nullptr);
//	}
//}



TEST( WeakSharedPtr, Copy )
{
	// Copy to longer-lived pointer
	{
		SharedPtr<int> source = DefaultCreator<int>::Create( 47 );
		{
			WeakSharedPtr<int> wsptr2;
			ASSERT_TRUE( wsptr2.Weaken() == nullptr );
			{
				WeakSharedPtr<int> wsptr1 = source;
				ASSERT_TRUE( wsptr1.Weaken() != nullptr );
				ASSERT_TRUE( *wsptr1.Weaken() == 47 );
				wsptr2 = wsptr1;
				ASSERT_TRUE( wsptr1.Weaken() != nullptr );
				ASSERT_TRUE( wsptr2.Weaken() != nullptr );
				ASSERT_TRUE( *wsptr2.Weaken() == 47 );
			}
			ASSERT_TRUE( wsptr2.Weaken() != nullptr );
			ASSERT_TRUE( *wsptr2.Weaken() == 47 );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}
