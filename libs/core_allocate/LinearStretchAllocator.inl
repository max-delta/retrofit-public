#pragma once
#include "LinearStretchAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

template<size_t MaxTotalSize, size_t Align>
inline LinearStretchAllocator<MaxTotalSize, Align>::LinearStretchAllocator( ExplicitDefaultConstruct )
	: mLinearAllocator( ExplicitDefaultConstruct{} )
{
	//
}



template<size_t MaxTotalSize, size_t Align>
inline LinearStretchAllocator<MaxTotalSize, Align>::~LinearStretchAllocator()
{
	if( GetCurrentHeapCount() > 0 )
	{
		RF_RETAIL_FATAL_MSG( "~LinearStretchAllocator()", "Non-zero heap allocation count" );
	}
}



template<size_t MaxTotalSize, size_t Align>
inline void* LinearStretchAllocator<MaxTotalSize, Align>::Allocate( size_t size )
{
	// Try linear first
	void* const linear = mLinearAllocator.Allocate( size );
	if( linear != nullptr )
	{
		return linear;
	}

	// Fallback to heap
	mHeapAllocations.fetch_add( 1, rftl::memory_order::memory_order_acq_rel );
	return ::operator new( size );
}



template<size_t MaxTotalSize, size_t Align>
inline void* LinearStretchAllocator<MaxTotalSize, Align>::Allocate( size_t size, size_t align )
{
	// Try linear first
	void* const linear = mLinearAllocator.Allocate( size, align );
	if( linear != nullptr )
	{
		return linear;
	}

	// Fallback to heap
	mHeapAllocations.fetch_add( 1, rftl::memory_order::memory_order_acq_rel );
	return ::operator new( size, align );
}



template<size_t MaxTotalSize, size_t Align>
inline void LinearStretchAllocator<MaxTotalSize, Align>::Delete( void* ptr )
{
	// Try linear first
	if( mLinearAllocator.IsPointerWithinAllocationRange( ptr ) )
	{
		mLinearAllocator.Delete( ptr );
		return;
	}

	// Fallback to heap
	delete ptr;
	mHeapAllocations.fetch_sub( 1, rftl::memory_order::memory_order_acq_rel );
	return;
}



template<size_t MaxTotalSize, size_t Align>
inline size_t LinearStretchAllocator<MaxTotalSize, Align>::GetCurrentHeapCount() const
{
	return mHeapAllocations.load( rftl::memory_order::memory_order_acquire );
}

///////////////////////////////////////////////////////////////////////////////
}
