#pragma once
#include "SingleAllocator.h"

#include "core/macros.h"
#include "rftl/atomic"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

template<size_t MaxTotalSize, size_t Align>
inline SingleAllocator<MaxTotalSize, Align>::SingleAllocator( ExplicitDefaultConstruct )
	: mHasAllocation( false )
{
	//
}



template<size_t MaxTotalSize, size_t Align>
inline void* SingleAllocator<MaxTotalSize, Align>::Allocate( size_t size )
{
	bool const hadAllocation = mHasAllocation.exchange( true, rftl::memory_order::memory_order_acq_rel );
	if( hadAllocation )
	{
		return nullptr;
	}
	if( size > MaxTotalSize )
	{
		mHasAllocation.store( false, rftl::memory_order::memory_order_release );
		return nullptr;
	}
	return &mStorageBytes[0];
}



template<size_t MaxTotalSize, size_t Align>
inline void* SingleAllocator<MaxTotalSize, Align>::Allocate( size_t size, size_t align )
{
	if( IsValidAlignment( align ) == false )
	{
		return nullptr;
	}

	if( align > kAlignment )
	{
		return nullptr;
	}

	return Allocate( size );
}



template<size_t MaxTotalSize, size_t Align>
inline void SingleAllocator<MaxTotalSize, Align>::Delete( void* ptr )
{
	RF_ASSERT( ptr == &mStorageBytes[0] );

	bool const hadAllocation = mHasAllocation.exchange( false, rftl::memory_order::memory_order_acq_rel );
	if( hadAllocation == false )
	{
		RF_DBGFAIL();
		return;
	}
}



template<size_t MaxTotalSize, size_t Align>
inline size_t SingleAllocator<MaxTotalSize, Align>::GetMaxSize() const
{
	return kMaxTotalSize;
}

///////////////////////////////////////////////////////////////////////////////
}}
