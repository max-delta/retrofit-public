#include "stdafx.h"

#include "core_allocate/LinearAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

TEST( LinearAllocator, Standalone )
{
	static constexpr size_t kSize = 8;
	static constexpr size_t kAlign = 2;
	LinearAllocator<kSize, kAlign> alloc{ ExplicitDefaultConstruct() };
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 0 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );

	void* const byteAllocation = alloc.Allocate( 1 );
	ASSERT_NE( byteAllocation, nullptr );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 2 );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );

	void* const quadAllocation = alloc.Allocate( 4 );
	ASSERT_NE( quadAllocation, nullptr );
	ASSERT_TRUE( quadAllocation > byteAllocation );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 );
	ASSERT_EQ( alloc.GetCurrentCount(), 2 );

	alloc.Delete( byteAllocation );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );

	alloc.Delete( quadAllocation );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );

	void* const overAllocation = alloc.Allocate( 4 );
	ASSERT_EQ( overAllocation, nullptr );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 6 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );
}



TEST( LinearAllocator, Leak )
{
	static constexpr size_t kSize = 8;
	static constexpr size_t kAlign = 2;
	LinearAllocator<kSize, kAlign> alloc{ ExplicitDefaultConstruct() };
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 0 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );

	void* const byteAllocation = alloc.Allocate( 1 );
	ASSERT_NE( byteAllocation, nullptr );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 2 );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );

	alloc.RelinquishAllAllocations();
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 2 );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );
}

///////////////////////////////////////////////////////////////////////////////
}
