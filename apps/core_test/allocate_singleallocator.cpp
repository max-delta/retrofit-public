#include "stdafx.h"

#include "core_allocate/SingleAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

TEST( SingleAllocator, Standalone )
{
	static constexpr size_t kSize = 8;
	SingleAllocator<kSize> alloc{ ExplicitDefaultConstruct() };
	for( uint64_t i = 0; i < 4; i++ )
	{
		ASSERT_EQ( alloc.GetMaxSize(), kSize );
		ASSERT_EQ( alloc.GetCurrentSize(), 0 );
		ASSERT_EQ( alloc.GetCurrentCount(), 0 );

		void* const allocation = alloc.Allocate( kSize );
		ASSERT_NE( allocation, nullptr );
		ASSERT_EQ( alloc.GetMaxSize(), kSize );
		ASSERT_EQ( alloc.GetCurrentSize(), kSize );
		ASSERT_EQ( alloc.GetCurrentCount(), 1 );

		void* const doubleAllocation = alloc.Allocate( kSize );
		ASSERT_EQ( doubleAllocation, nullptr );
		ASSERT_EQ( alloc.GetMaxSize(), kSize );
		ASSERT_EQ( alloc.GetCurrentSize(), kSize );
		ASSERT_EQ( alloc.GetCurrentCount(), 1 );

		alloc.Delete( allocation );
		ASSERT_EQ( alloc.GetMaxSize(), kSize );
		ASSERT_EQ( alloc.GetCurrentSize(), 0 );
		ASSERT_EQ( alloc.GetCurrentCount(), 0 );
	}
}



TEST( SingleAllocator, Leak )
{
	static constexpr size_t kSize = 8;
	SingleAllocator<kSize> alloc{ ExplicitDefaultConstruct() };
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 0 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );

	void* const allocation = alloc.Allocate( kSize );
	ASSERT_NE( allocation, nullptr );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );

	alloc.RelinquishAllAllocations();
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );
}

///////////////////////////////////////////////////////////////////////////////
}
