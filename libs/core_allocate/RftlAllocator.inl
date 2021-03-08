#pragma once
#include "RftlAllocator.h"

#include "rftl/limits"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
inline RftlAllocator<T>::RftlAllocator( Allocator& allocator )
	: mAllocator( allocator )
{
	//
}



template<typename T>
template<typename U>
inline RftlAllocator<T>::RftlAllocator( RftlAllocator<U> const& rhs )
	: mAllocator( rhs.mAllocator )
{
	//
}



template<typename T>
template<typename U>
inline RftlAllocator<T>::RftlAllocator( RftlAllocator<U>&& rhs )
	: mAllocator( rhs.mAllocator )
{
	//
}



template<typename T>
inline typename RftlAllocator<T>::pointer RftlAllocator<T>::allocate( size_type count ) noexcept
{
	return reinterpret_cast<typename RftlAllocator<T>::pointer>( mAllocator.Allocate( count * sizeof( T ) ) );
}



template<typename T>
inline typename RftlAllocator<T>::pointer RftlAllocator<T>::allocate( size_type count, const_void_pointer hint ) noexcept
{
	(void)hint;
	return allocate( count * sizeof( T ) );
}



template<typename T>
inline void RftlAllocator<T>::deallocate( pointer ptr, size_type count ) noexcept
{
	(void)count;
	return mAllocator.Delete( ptr );
}



template<typename T>
template<class U, class... Args>
inline void RftlAllocator<T>::construct( U* ptr, Args&&... args )
{
	new( ptr ) U( args... );
}



template<typename T>
template<class U>
inline void RftlAllocator<T>::destroy( U* ptr )
{
	ptr->~U();
}



template<typename T>
inline typename RftlAllocator<T>::size_type RftlAllocator<T>::max_size()
{
	return mAllocator.GetMaxSize();
}



template<typename T>
inline typename RftlAllocator<T>::ThisType RftlAllocator<T>::select_on_container_copy_construction()
{
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}
