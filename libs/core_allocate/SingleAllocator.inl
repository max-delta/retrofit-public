#pragma once
#include "SingleAllocator.h"

#include "core/macros.h"
#include "rftl/atomic"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T, size_t MaxTotalSize>
inline SingleAllocator<T, MaxTotalSize>::SingleAllocator( ExplicitDefaultConstruct )
	: mHasAllocation( false )
{
	//
}



template<typename T, size_t MaxTotalSize>
inline typename SingleAllocator<T, MaxTotalSize>::pointer SingleAllocator<T, MaxTotalSize>::allocate( size_type count ) noexcept
{
	bool const hadAllocation = mHasAllocation.exchange( true, rftl::memory_order::memory_order_acq_rel );
	if( hadAllocation )
	{
		return nullptr;
	}
	if( count != 1 )
	{
		mHasAllocation.store( false, rftl::memory_order::memory_order_release );
		return nullptr;
	}
	return reinterpret_cast<pointer>( &mStorageBytes[0] );
}



template<typename T, size_t MaxTotalSize>
inline typename SingleAllocator<T, MaxTotalSize>::pointer SingleAllocator<T, MaxTotalSize>::allocate( size_type count, const_void_pointer hint ) noexcept
{
	(void)hint;
	return allocate( count );
}



template<typename T, size_t MaxTotalSize>
inline void SingleAllocator<T, MaxTotalSize>::deallocate( pointer ptr, size_type count ) noexcept
{
	RF_ASSERT( ptr == reinterpret_cast<pointer>( &mStorageBytes[0] ) );
	RF_ASSERT( count == 1 );

	bool const hadAllocation = mHasAllocation.exchange( false, rftl::memory_order::memory_order_acq_rel );
	if( hadAllocation == false )
	{
		RF_DBGFAIL();
		return;
	}
}



template<typename T, size_t MaxTotalSize>
template<class U, class... Args>
inline void SingleAllocator<T, MaxTotalSize>::construct( U* ptr, Args&&... args )
{
	new( ptr ) U( args... );
}



template<typename T, size_t MaxTotalSize>
template<class U>
inline void SingleAllocator<T, MaxTotalSize>::destroy( U* ptr )
{
	ptr->~U();
}



template<typename T, size_t MaxTotalSize>
inline typename SingleAllocator<T, MaxTotalSize>::size_type SingleAllocator<T, MaxTotalSize>::max_size()
{
	return kMaxTotalSize;
}



template<typename T, size_t MaxTotalSize>
inline typename SingleAllocator<T, MaxTotalSize>::ThisType SingleAllocator<T, MaxTotalSize>::select_on_container_copy_construction()
{
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}
