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
template<class U, class ...Args>
inline void SingleAllocator<T, MaxTotalSize>::construct( U * ptr, Args && ...args )
{
	new ( ptr )U( args... );
}



template<typename T, size_t MaxTotalSize>
template<class U>
inline void SingleAllocator<T, MaxTotalSize>::destroy( U * ptr )
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
namespace std {
///////////////////////////////////////////////////////////////////////////////

template<typename T, size_t MaxTotalSize>
inline typename rftl::allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::pointer
rftl::allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::allocate( allocator_type & alloc, size_type count )
{
	return alloc.allocate( count );
}



template<typename T, size_t MaxTotalSize>
inline typename rftl::allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::pointer
allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::allocate( allocator_type & alloc, size_type count, const_void_pointer hint )
{
	return alloc.allocate( count, hint );
}



template<typename T, size_t MaxTotalSize>
inline void allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::deallocate( allocator_type & alloc, pointer ptr, size_type count )
{
	return alloc.deallocate( alloc, ptr, count );
}



template<typename T, size_t MaxTotalSize>
template<class U, class ...Args>
inline void allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::construct( allocator_type & alloc, U * ptr, Args && ...args )
{
	alloc.construct( ptr, args );
}



template<typename T, size_t MaxTotalSize>
template<class U>
inline void allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::destroy( allocator_type & alloc, U * ptr )
{
	alloc.destroy( ptr );
}



template<typename T, size_t MaxTotalSize>
inline typename rftl::allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::size_type
allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::max_size( allocator_type const & alloc )
{
	return alloc.max_size();
}



template<typename T, size_t MaxTotalSize>
inline typename rftl::allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::allocator_type
allocator_traits<RF::SingleAllocator<T, MaxTotalSize>>::select_on_container_copy_construction( allocator_type const & alloc )
{
	return alloc.select_on_container_copy_construction();
}

///////////////////////////////////////////////////////////////////////////////
}
