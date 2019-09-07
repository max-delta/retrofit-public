#include "stdafx.h"

#include "core_allocate/SingleAllocator.h"

#include "rftl/utility"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

TEST( SingleAllocator, Standalone )
{
	static constexpr size_t kSize = 8;
	SingleAllocator<kSize> alloc{ ExplicitDefaultConstruct() };
	for( uint64_t i = 0; i < 4; i++ )
	{
		void* const allocation = alloc.Allocate( kSize );
		ASSERT_NE( allocation, nullptr );

		void* const doubleAllocation = alloc.Allocate( kSize );
		ASSERT_EQ( doubleAllocation, nullptr );

		alloc.Delete( allocation );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
