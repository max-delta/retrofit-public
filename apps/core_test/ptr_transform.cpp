#include "stdafx.h"

#include "core/ptr/ptr_transform.h"
#include "core/ptr/weak_shared_ptr.h"
#include "core/ptr/default_creator.h"

#include "rftl/cstdlib"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( UniquePtr, TransformToShared )
{
	WeakPtr<int> wptr;
	ASSERT_TRUE( wptr == nullptr );
	{
		SharedPtr<int> sptr;
		ASSERT_TRUE( sptr == nullptr );
		{
			UniquePtr<int> uptr = DefaultCreator<int>::Create( 47 );
			ASSERT_TRUE( uptr != nullptr );
			wptr = uptr;
			ASSERT_TRUE( wptr != nullptr );
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
	// This is a dangerous transformation, which can be made to fail by using
	//  these flags to violate the ownership contracts
	constexpr bool kCauseFailureOnCreate = false;
	constexpr bool kCauseFailureOnDestroy = false;
	WeakSharedPtr<int> unsafe_wsptr;
	SharedPtr<int> unsafe_sptr;

	WeakPtr<int> wptr;
	ASSERT_TRUE( wptr == nullptr );
	{
		UniquePtr<int> uptr;
		ASSERT_TRUE( uptr == nullptr );
		{
			SharedPtr<int> sptr = DefaultCreator<int>::Create( 47 );
			ASSERT_TRUE( sptr != nullptr );
			wptr = sptr;
			ASSERT_TRUE( wptr != nullptr );
			{
				if( kCauseFailureOnCreate )
				{
					// This creates a shared scenario on create
					// NOTE: This is detectable
					unsafe_sptr = sptr;
				}
				if( kCauseFailureOnDestroy )
				{
					// This creates a lurking danger that is hidden on create
					// NOTE: This is not detectable at time of writing
					unsafe_wsptr = sptr;
				}
				PtrTransformer<int>::PerformUncheckedTransformation( rftl::move( sptr ), uptr );
				ASSERT_TRUE( sptr == nullptr );
				ASSERT_TRUE( uptr != nullptr );
				ASSERT_TRUE( wptr != nullptr );
				if( kCauseFailureOnDestroy )
				{
					// This creates a shared scenario on destroy
					// NOTE: This is detectable
					unsafe_sptr = unsafe_wsptr.Lock();
				}
			}
		}
		ASSERT_TRUE( uptr != nullptr );
		ASSERT_TRUE( wptr != nullptr );
	}
	ASSERT_TRUE( wptr == nullptr );
}



TEST( UniquePtr, TransformToVoid )
{
	UniquePtr<int> uptr1 = DefaultCreator<int>::Create( 47 );
	ASSERT_TRUE( uptr1 != nullptr );

	UniquePtr<void> uptr2;
	PtrTransformer<int>::PerformVoidTransformation( rftl::move( uptr1 ), uptr2 );
	ASSERT_TRUE( uptr1 == nullptr );
	ASSERT_TRUE( uptr2 != nullptr );

	int const* const val = reinterpret_cast<int const*>( static_cast<void const*>( uptr2 ) );
	ASSERT_TRUE( val != nullptr );
	ASSERT_EQ( *val, 47 );
}

///////////////////////////////////////////////////////////////////////////////
}
