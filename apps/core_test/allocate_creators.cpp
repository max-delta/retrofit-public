#include "stdafx.h"

#include "core_allocate/LinearAllocator.h"
#include "core_allocate/DefaultAllocCreator.h"

#include "core/ptr/unique_ptr.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

TEST( AllocCreators, Split )
{
	static constexpr size_t kSize = 256;
	static constexpr size_t kAlign = 2;
	AllocatorT<LinearAllocator<kSize, kAlign>> alloc{ ExplicitDefaultConstruct() };
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 0 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );

	// Need a large enough allocation for the split allocator to be used
	struct LargeAlloc
	{
		uint8_t unused[128];
	};
	UniquePtr<LargeAlloc> largeAlloc = SplitAllocCreator<LargeAlloc>::Create( alloc );

	ASSERT_NE( largeAlloc, nullptr );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	size_t const highWater = alloc.GetCurrentSize();
	ASSERT_TRUE( highWater > sizeof( LargeAlloc ) );
	ASSERT_EQ( alloc.GetCurrentCount(), 2 );

	largeAlloc = nullptr;

	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), highWater );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );
}



TEST( AllocCreators, Entwined )
{
	static constexpr size_t kSize = 128;
	static constexpr size_t kAlign = 2;
	AllocatorT<LinearAllocator<kSize, kAlign>> alloc{ ExplicitDefaultConstruct() };
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), 0 );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );

	UniquePtr<uint32_t> allocation = EntwinedAllocCreator<uint32_t>::Create( alloc, 7u );

	ASSERT_NE( allocation, nullptr );
	ASSERT_EQ( *allocation, 7u );
	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	size_t const highWater = alloc.GetCurrentSize();
	ASSERT_TRUE( highWater > sizeof( uint32_t ) );
	ASSERT_EQ( alloc.GetCurrentCount(), 1 );

	allocation = nullptr;

	ASSERT_EQ( alloc.GetMaxSize(), kSize );
	ASSERT_EQ( alloc.GetCurrentSize(), highWater );
	ASSERT_EQ( alloc.GetCurrentCount(), 0 );
}

///////////////////////////////////////////////////////////////////////////////
}
