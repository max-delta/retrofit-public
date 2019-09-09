#include "stdafx.h"

#include "core_platform/MemoryRegion.h"


namespace RF { namespace platform {
///////////////////////////////////////////////////////////////////////////////

TEST( MemoryRegion, ExclusiveData )
{
	void* const region = AcquireLargeExclusiveDataRegion( 4097, "Slightly over a page" );
	ASSERT_NE( region, nullptr );

	//*(reinterpret_cast<uint8_t*>( region ) - 1) = 1; // Out of bounds
	*(reinterpret_cast<uint8_t*>( region ) + 0) = 1;
	//*(reinterpret_cast<uint8_t*>( region ) + 8192) = 1; // Out of bounds

	ReleaseLargeExclusiveDataRegion( region );
}

///////////////////////////////////////////////////////////////////////////////
}}
