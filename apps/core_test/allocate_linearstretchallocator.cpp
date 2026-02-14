#include "stdafx.h"

#include "core_allocate/LinearStretchAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

TEST( LinearStretchAllocator, Standalone )
{
	static constexpr size_t kSize = 8;
	static constexpr size_t kAlign = 2;
	LinearStretchAllocator<kSize, kAlign> alloc{ ExplicitDefaultConstruct() };
	ASSERT_EQ( alloc.GetCurrentHeapCount(), 0 );

	void* const byteAllocation = alloc.Allocate( 1 );
	ASSERT_NE( byteAllocation, nullptr );
	ASSERT_EQ( alloc.GetCurrentHeapCount(), 0 );

	void* const quadAllocation = alloc.Allocate( 4 );
	ASSERT_NE( quadAllocation, nullptr );
	ASSERT_TRUE( quadAllocation > byteAllocation );
	ASSERT_EQ( alloc.GetCurrentHeapCount(), 0 );

	alloc.Delete( byteAllocation );
	ASSERT_EQ( alloc.GetCurrentHeapCount(), 0 );

	alloc.Delete( quadAllocation );
	ASSERT_EQ( alloc.GetCurrentHeapCount(), 0 );

	void* const overAllocation = alloc.Allocate( 4 );
	ASSERT_NE( overAllocation, nullptr );
	ASSERT_EQ( alloc.GetCurrentHeapCount(), 1 );

	alloc.Delete( overAllocation );
	ASSERT_EQ( alloc.GetCurrentHeapCount(), 0 );
}

///////////////////////////////////////////////////////////////////////////////
}
