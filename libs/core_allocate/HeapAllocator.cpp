#include "stdafx.h"

#include "HeapAllocator.h"

#include "core_allocate/AlignmentHelpers.h"
#include "core_platform/MemoryRegion.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

HeapAllocator::HeapAllocator( ExplicitDefaultConstruct )
	: mHeapHandle( platform::AcquireLargeDynamicHeapHandle( 0, nullptr, true ) )
{
	//
}



HeapAllocator::~HeapAllocator()
{
	if( GetCurrentCount() > 0 && mHasRelinquishedAllAllocations.load( rftl::memory_order::memory_order_acquire ) == false )
	{
		RF_RETAIL_FATAL_MSG( "~HeapAllocator()", "Non-zero allocation count" );
	}

	platform::ReleaseLargeDynamicHeapHandle( mHeapHandle );
}



void* HeapAllocator::Allocate( size_t size )
{
	if( mHasRelinquishedAllAllocations.load( rftl::memory_order::memory_order_acquire ) )
	{
		RF_RETAIL_FATAL_MSG( "HeapAllocator::Allocate(...)", "Allocations relinquished" );
	}

	mAllocCount.fetch_add( 1, rftl::memory_order::memory_order_acq_rel );
	return platform::AllocateInLargeDynamicHeap( mHeapHandle, size );
}



void* HeapAllocator::Allocate( size_t size, size_t align )
{
	if( IsValidAlignment( align ) == false )
	{
		return nullptr;
	}

	return Allocate( size );
}



void HeapAllocator::Delete( void* ptr )
{
	mAllocCount.fetch_sub( 1, rftl::memory_order::memory_order_acq_rel );
	platform::DeleteInLargeDynamicHeap( mHeapHandle, ptr );
}



size_t HeapAllocator::GetMaxSize() const
{
	return rftl::numeric_limits<size_t>::max();
}



size_t HeapAllocator::GetCurrentSize() const
{
	return platform::QueryLargeDynamicHeapSize( mHeapHandle );
}



size_t HeapAllocator::GetCurrentCount() const
{
	return mAllocCount.load( rftl::memory_order::memory_order_acquire );
}



void HeapAllocator::RelinquishAllAllocations()
{
	mHasRelinquishedAllAllocations.store( true, rftl::memory_order::memory_order_release );
}

///////////////////////////////////////////////////////////////////////////////
}
