#pragma once
#include "SingleAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

template<size_t MaxTotalSize, size_t Align>
inline SingleAllocator<MaxTotalSize, Align>::SingleAllocator( ExplicitDefaultConstruct )
	: mHasAllocation( false )
{
	//
}



template<size_t MaxTotalSize, size_t Align>
inline SingleAllocator<MaxTotalSize, Align>::~SingleAllocator()
{
	if( GetCurrentCount() > 0 && mHasRelinquishedAllAllocations.load( rftl::memory_order::memory_order_acquire ) == false )
	{
		RF_RETAIL_FATAL_MSG( "~SingleAllocator()", "Non-zero allocation count" );
	}
}



template<size_t MaxTotalSize, size_t Align>
inline void* SingleAllocator<MaxTotalSize, Align>::Allocate( size_t size )
{
	if( mHasRelinquishedAllAllocations.load( rftl::memory_order::memory_order_acquire ) )
	{
		RF_RETAIL_FATAL_MSG( "SingleAllocator::Allocate(...)", "Allocations relinquished" );
	}

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



template<size_t MaxTotalSize, size_t Align>
inline size_t SingleAllocator<MaxTotalSize, Align>::GetCurrentSize() const
{
	return kMaxTotalSize * GetCurrentCount();
}



template<size_t MaxTotalSize, size_t Align>
inline size_t SingleAllocator<MaxTotalSize, Align>::GetCurrentCount() const
{
	if( mHasAllocation.load( rftl::memory_order::memory_order_acquire ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



template<size_t MaxTotalSize, size_t Align>
inline void SingleAllocator<MaxTotalSize, Align>::RelinquishAllAllocations()
{
	mHasRelinquishedAllAllocations.store( true, rftl::memory_order::memory_order_release );
}

///////////////////////////////////////////////////////////////////////////////
}
