#include "stdafx.h"

#include "core_allocate/LinearStretchAllocator.h"
#include "core_allocate/SingleAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

TEST( LinearStretchAllocator, Standalone )
{
	static constexpr size_t kStretchSize = 6;
	static constexpr size_t kStretchAlign = 1;
	AllocatorT<SingleAllocator<kStretchSize, kStretchAlign>> tempSingle{ ExplicitDefaultConstruct{} };

	static constexpr size_t kSize = 8;
	static constexpr size_t kAlign = 2;
	LinearStretchAllocator<kSize, kAlign> alloc{ tempSingle };
	ASSERT_EQ( alloc.GetMaxSize(), rftl::numeric_limits<size_t>::max() );
	ASSERT_EQ( alloc.GetCurrentSize(), 0 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );
	ASSERT_EQ( alloc.GetCurrentStretchCount(), 0 );

	void* const byteAllocation = alloc.Allocate( 1 );
	ASSERT_NE( byteAllocation, nullptr );
	ASSERT_EQ( alloc.GetMaxSize(), rftl::numeric_limits<size_t>::max() );
	ASSERT_EQ( alloc.GetCurrentSize(), 2 );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );
	ASSERT_EQ( alloc.GetCurrentStretchCount(), 0 );

	void* const quadAllocation = alloc.Allocate( 4 );
	ASSERT_NE( quadAllocation, nullptr );
	ASSERT_TRUE( quadAllocation > byteAllocation );
	ASSERT_EQ( alloc.GetMaxSize(), rftl::numeric_limits<size_t>::max() );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 );
	ASSERT_EQ( alloc.GetCurrentCount(), 2 );
	ASSERT_EQ( alloc.GetCurrentStretchCount(), 0 );

	alloc.Delete( byteAllocation );
	ASSERT_EQ( alloc.GetMaxSize(), rftl::numeric_limits<size_t>::max() );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );
	ASSERT_EQ( alloc.GetCurrentStretchCount(), 0 );

	alloc.Delete( quadAllocation );
	ASSERT_EQ( alloc.GetMaxSize(), rftl::numeric_limits<size_t>::max() );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );
	ASSERT_EQ( alloc.GetCurrentStretchCount(), 0 );

	void* const overAllocation = alloc.Allocate( 4 );
	ASSERT_NE( overAllocation, nullptr );
	ASSERT_EQ( alloc.GetMaxSize(), rftl::numeric_limits<size_t>::max() );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 + kStretchSize );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );
	ASSERT_EQ( alloc.GetCurrentStretchCount(), 1 );

	alloc.Delete( overAllocation );
	ASSERT_EQ( alloc.GetMaxSize(), rftl::numeric_limits<size_t>::max() );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );
	ASSERT_EQ( alloc.GetCurrentStretchCount(), 0 );
}

///////////////////////////////////////////////////////////////////////////////
}
