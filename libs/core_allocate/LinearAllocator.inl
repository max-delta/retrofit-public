#pragma once
#include "LinearAllocator.h"

#include "core_math/math_clamps.h"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

template<size_t MaxTotalSize, size_t Align>
inline LinearAllocator<MaxTotalSize, Align>::LinearAllocator( ExplicitDefaultConstruct )
{
	//
}



template<size_t MaxTotalSize, size_t Align>
inline void* LinearAllocator<MaxTotalSize, Align>::Allocate( size_t size )
{
	size_t const alignedSize = math::SnapHighest( size, kAlignment );

	size_t const startingOffset = mBytesAllocated.fetch_add( alignedSize, rftl::memory_order::memory_order_acq_rel );
	if( startingOffset >= kMaxTotalSize )
	{
		// Already beyond buffer
		// NOTE: This may happen if two or more allocations both fail at the same time
		mBytesAllocated.fetch_sub( alignedSize, rftl::memory_order::memory_order_acq_rel );
		return nullptr;
	}

	// NOTE: Previous check ensures this will be >= 0
	size_t const availableBytes = kMaxTotalSize - startingOffset;
	if( availableBytes < alignedSize )
	{
		// Not enough buffer
		// NOTE: This may happen if two or more allocations both fail at the same time
		mBytesAllocated.fetch_sub( alignedSize, rftl::memory_order::memory_order_acq_rel );
		return nullptr;
	}

	mCurrentAllocations.fetch_add( 1, rftl::memory_order::memory_order_acq_rel );
	return &mStorageBytes[startingOffset];
}



template<size_t MaxTotalSize, size_t Align>
inline void* LinearAllocator<MaxTotalSize, Align>::Allocate( size_t size, size_t align )
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
inline void LinearAllocator<MaxTotalSize, Align>::Delete( void* ptr )
{
	RF_ASSERT( ptr >= &mStorageBytes[0] );
	RF_ASSERT( ptr <= &mStorageBytes[kMaxTotalSize - 1] );

	// NOTE: No deletion actually happens
	mCurrentAllocations.fetch_sub( 1, rftl::memory_order::memory_order_acq_rel );
}



template<size_t MaxTotalSize, size_t Align>
inline size_t LinearAllocator<MaxTotalSize, Align>::GetMaxSize() const
{
	return kMaxTotalSize;
}



template<size_t MaxTotalSize, size_t Align>
inline size_t LinearAllocator<MaxTotalSize, Align>::GetCurrentSize() const
{
	return mBytesAllocated.load( rftl::memory_order::memory_order_acquire );
}



template<size_t MaxTotalSize, size_t Align>
inline size_t LinearAllocator<MaxTotalSize, Align>::GetCurrentCount() const
{
	return mCurrentAllocations.load( rftl::memory_order::memory_order_acquire );
}

///////////////////////////////////////////////////////////////////////////////
}}
