#pragma once
#include "core_allocate/Allocator.h"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

// RFTL interface for talking to allocators
template<typename T>
class RftlAllocator
{
	//
	// Friends
private:
	template<typename U>
	friend class RftlAllocator;


	//
	// Types and constants
public:
	using ThisType = RftlAllocator<T>;


	//
	// RFTL types
public:
	using value_type = T;

	using pointer = value_type*;
	using const_pointer = value_type const*;
	using void_pointer = void*;
	using const_void_pointer = void const*;

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_copy_assignment = rftl::true_type;
	using propagate_on_container_move_assignment = rftl::true_type;
	using propagate_on_container_swap = rftl::true_type;

	using is_always_equal = rftl::false_type;

	template<typename U>
	struct rebind
	{
		using other = RF::alloc::RftlAllocator<U>;
	};


	//
	// Public methods
public:
	explicit RftlAllocator( Allocator& allocator );

	RftlAllocator& operator=( RftlAllocator<T> const& ) = delete;


	//
	// RFTL methods
public:
	template<typename U>
	RftlAllocator( RftlAllocator<U> const& rhs );
	template<typename U>
	RftlAllocator( RftlAllocator<U>&& rhs );

	pointer allocate( size_type count ) noexcept;
	pointer allocate( size_type count, const_void_pointer hint ) noexcept;
	void deallocate( pointer ptr, size_type count ) noexcept;
	template<class U, class... Args>
	void construct( U* ptr, Args&&... args );
	template<class U>
	void destroy( U* ptr );
	size_type max_size();
	ThisType select_on_container_copy_construction();


	//
	// Private data
private:
	Allocator& mAllocator;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "RftlAllocator.inl"
