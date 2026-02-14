#pragma once
#include "LinearStretchAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

template<size_t LinearSize, size_t Align>
inline LinearStretchAllocator<LinearSize, Align>::LinearStretchAllocator( Allocator& stretchAllocator )
	: mLinearAllocator( ExplicitDefaultConstruct{} )
	, mStretchAllocator( stretchAllocator )
{
	//
}



template<size_t LinearSize, size_t Align>
inline LinearStretchAllocator<LinearSize, Align>::~LinearStretchAllocator() = default;



template<size_t LinearSize, size_t Align>
inline void* LinearStretchAllocator<LinearSize, Align>::Allocate( size_t size )
{
	// Try linear first
	void* const linear = mLinearAllocator.Allocate( size );
	if( linear != nullptr )
	{
		return linear;
	}

	// Fallback to stretch
	return mStretchAllocator.Allocate( size );
}



template<size_t LinearSize, size_t Align>
inline void* LinearStretchAllocator<LinearSize, Align>::Allocate( size_t size, size_t align )
{
	// Try linear first
	void* const linear = mLinearAllocator.Allocate( size, align );
	if( linear != nullptr )
	{
		return linear;
	}

	// Fallback to stretch
	return mStretchAllocator.Allocate( size, align );
}



template<size_t LinearSize, size_t Align>
inline void LinearStretchAllocator<LinearSize, Align>::Delete( void* ptr )
{
	// Try linear first
	if( mLinearAllocator.IsPointerWithinAllocationRange( ptr ) )
	{
		mLinearAllocator.Delete( ptr );
		return;
	}

	// Fallback to stretch
	mStretchAllocator.Delete( ptr );
	return;
}



template<size_t LinearSize, size_t Align>
inline size_t LinearStretchAllocator<LinearSize, Align>::GetMaxSize() const
{
	return rftl::numeric_limits<size_t>::max();
}



template<size_t LinearSize, size_t Align>
inline size_t LinearStretchAllocator<LinearSize, Align>::GetCurrentSize() const
{
	return mLinearAllocator.GetCurrentSize() + mStretchAllocator.GetCurrentSize();
}



template<size_t LinearSize, size_t Align>
inline size_t LinearStretchAllocator<LinearSize, Align>::GetCurrentCount() const
{
	return mLinearAllocator.GetCurrentCount() + GetCurrentStretchCount();
}



template<size_t LinearSize, size_t Align>
inline void LinearStretchAllocator<LinearSize, Align>::RelinquishAllAllocations()
{
	mLinearAllocator.RelinquishAllAllocations();
	mStretchAllocator.RelinquishAllAllocations();
}



template<size_t LinearSize, size_t Align>
inline size_t LinearStretchAllocator<LinearSize, Align>::GetCurrentStretchCount() const
{
	return mStretchAllocator.GetCurrentCount();
}

///////////////////////////////////////////////////////////////////////////////
}
