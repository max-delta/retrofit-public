#include "stdafx.h"

#include "HeapAllocator.h"

#include "core_allocate/AlignmentHelpers.h"
#include "core_platform/MemoryRegion.h"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

HeapAllocator::HeapAllocator( ExplicitDefaultConstruct )
	: mHeapHandle( platform::AcquireLargeDynamicHeapHandle( 0, nullptr, true ) )
{
	//
}



void* HeapAllocator::Allocate( size_t size )
{
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

///////////////////////////////////////////////////////////////////////////////
}}
