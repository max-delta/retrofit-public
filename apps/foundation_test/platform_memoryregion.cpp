#include "stdafx.h"

#include "core_platform/MemoryRegion.h"


namespace RF { namespace platform {
///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRegion, ExclusiveData )
{
	void* const region = AcquireLargeExclusiveDataRegion( 4097, "Slightly over a page" );
	ASSERT_NE( region, nullptr );

	//*( reinterpret_cast<uint8_t*>( region ) - 1 ) = 1; // Out of bounds
	*( reinterpret_cast<uint8_t*>( region ) + 0 ) = 1;
	//*( reinterpret_cast<uint8_t*>( region ) + 8192 ) = 1; // Out of bounds

	ReleaseLargeExclusiveDataRegion( region );
}



TEST( MemoryRegion, Heap )
{
	void const* const heapHandle = AcquireLargeDynamicHeapHandle( 4096, "Custom heap test", true );
	ASSERT_NE( heapHandle, nullptr );
	ASSERT_EQ( QueryLargeDynamicHeapSize( heapHandle ), 0 );

	void* const allocation = AllocateInLargeDynamicHeap( heapHandle, 5 );
	ASSERT_NE( allocation, nullptr );
	ASSERT_EQ( QueryLargeDynamicHeapSize( heapHandle ), 5 );

	*( reinterpret_cast<uint8_t*>( allocation ) + 0 ) = 1;
	*( reinterpret_cast<uint8_t*>( allocation ) + 1 ) = 2;
	*( reinterpret_cast<uint8_t*>( allocation ) + 2 ) = 3;
	*( reinterpret_cast<uint8_t*>( allocation ) + 3 ) = 4;
	*( reinterpret_cast<uint8_t*>( allocation ) + 4 ) = 5;

	DeleteInLargeDynamicHeap( heapHandle, allocation );
	ASSERT_EQ( QueryLargeDynamicHeapSize( heapHandle ), 0 );

	ReleaseLargeDynamicHeapHandle( heapHandle );
}

///////////////////////////////////////////////////////////////////////////////
}}
