#include "stdafx.h"

#include "core/ptr/ptr_transform.h"
#include "core/ptr/entwined_creator.h"

#include "rftl/cstdlib"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST(UniquePtr, TransformToShared)
{
	WeakPtr<int> wptr;
	ASSERT_TRUE( wptr == nullptr );
	{
		SharedPtr<int> sptr;
		ASSERT_TRUE( sptr == nullptr );
		{
			UniquePtr<int> uptr = EntwinedCreator<int>::Create(47);
			ASSERT_TRUE( uptr != nullptr );
			wptr = uptr;
			ASSERT_TRUE(wptr != nullptr);
			{
				PtrTransformer<int>::PerformTransformation( rftl::move( uptr ), sptr );
				ASSERT_TRUE( uptr == nullptr );
				ASSERT_TRUE( sptr != nullptr );
				ASSERT_TRUE( wptr != nullptr );
			}
		}
		ASSERT_TRUE( sptr != nullptr );
		ASSERT_TRUE( wptr != nullptr );
	}
	ASSERT_TRUE( wptr == nullptr );
}



TEST( SharedPtr, TransformToUnique )
{
	WeakPtr<int> wptr;
	ASSERT_TRUE( wptr == nullptr );
	{
		UniquePtr<int> uptr;
		ASSERT_TRUE( uptr == nullptr );
		{
			SharedPtr<int> sptr = EntwinedCreator<int>::Create( 47 );
			ASSERT_TRUE( sptr != nullptr );
			wptr = sptr;
			ASSERT_TRUE( wptr != nullptr );
			{
				PtrTransformer<int>::PerformUncheckedTransformation( rftl::move( sptr ), uptr );
				ASSERT_TRUE( sptr == nullptr );
				ASSERT_TRUE( uptr != nullptr );
				ASSERT_TRUE( wptr != nullptr );
			}
		}
		ASSERT_TRUE( uptr != nullptr );
		ASSERT_TRUE( wptr != nullptr );
	}
	ASSERT_TRUE( wptr == nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}
