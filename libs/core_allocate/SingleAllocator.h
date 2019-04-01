#pragma once
#include "core/meta/ConstructorOverload.h"

#include "rftl/atomic"
#include "rftl/memory"
#include "rftl/type_traits"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T, size_t MaxTotalSize>
class SingleAllocator
{
	//
	// Types and constants
public:
	using ThisType = SingleAllocator<T, MaxTotalSize>;
	static constexpr size_t kMaxTotalSize = MaxTotalSize;
	static_assert( sizeof( T ) <= MaxTotalSize, "Trying to store type that is too large for storage" );


	//
	// STL types
public:
	using value_type = T;

	using pointer = value_type*;
	using const_pointer = value_type const*;
	using void_pointer = void*;
	using const_void_pointer = void const*;

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_copy_assignment = rftl::false_type;
	using propagate_on_container_move_assignment = rftl::false_type;
	using propagate_on_container_swap_assignment = rftl::false_type;

	using is_always_equal = rftl::false_type;

	template<typename U>
	struct rebind
	{
		using other = RF::SingleAllocator<U, MaxTotalSize>;
	};


	//
	// Public methods
public:
	// No implicit default construction, to prevent accidental creation
	SingleAllocator() = delete;
	explicit SingleAllocator( ExplicitDefaultConstruct );


	//
	// STL methods
public:
	// NOTE: Intentionally violating the STL allocator contract, preventing
	//  this allocator from being copied or moved
	template<typename U, size_t OtherMaxTotalSize>
	SingleAllocator( SingleAllocator<U, OtherMaxTotalSize> const& rhs ) = delete;
	template<typename U, size_t OtherMaxTotalSize>
	SingleAllocator( SingleAllocator<U, OtherMaxTotalSize>&& rhs ) = delete;

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
	volatile rftl::atomic_bool mHasAllocation;
	uint8_t mStorageBytes[kMaxTotalSize];
};

///////////////////////////////////////////////////////////////////////////////
}

#include "SingleAllocator.inl"
