#pragma once
#include "core/ConstructorOverload.h"

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

	using pointer = value_type *;
	using const_pointer = value_type const *;
	using void_pointer = void *;
	using const_void_pointer = void const *;

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
	SingleAllocator( SingleAllocator<U, OtherMaxTotalSize> && rhs ) = delete;

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
namespace rftl {
///////////////////////////////////////////////////////////////////////////////
template<typename T, size_t MaxTotalSize>
struct allocator_traits< RF::SingleAllocator<T, MaxTotalSize> >
{
	using allocator_type = RF::SingleAllocator<T, MaxTotalSize>;
	using value_type = typename allocator_type::value_type;

	using pointer = typename allocator_type::pointer;
	using const_pointer = typename allocator_type::const_pointer;
	using void_pointer = typename allocator_type::void_pointer;
	using const_void_pointer = typename allocator_type::const_void_pointer;

	using difference_type = typename allocator_type::difference_type;
	using size_type = typename allocator_type::size_type;

	using propagate_on_container_copy_assignment = typename allocator_type::propagate_on_container_copy_assignment;
	using propagate_on_container_move_assignment = typename allocator_type::propagate_on_container_move_assignment;
	using propagate_on_container_swap_assignment = typename allocator_type::propagate_on_container_swap_assignment;

	using is_always_equal = typename allocator_type::is_always_equal;

	template<class U>
	using rebind_alloc = RF::SingleAllocator<U, MaxTotalSize>;

	template<class U>
	using rebind_traits = allocator_traits<RF::SingleAllocator<U, MaxTotalSize>>;

	static pointer allocate( allocator_type& alloc, size_type count );
	static pointer allocate( allocator_type& alloc, size_type count, const_void_pointer hint );
	static void deallocate( allocator_type& alloc, pointer ptr, size_type count );
	template<class U, class... Args>
	static void construct( allocator_type& alloc, U* ptr, Args&&... args );
	template<class U>
	static void destroy( allocator_type& alloc, U* ptr );
	static size_type max_size( allocator_type const& alloc );
	static allocator_type select_on_container_copy_construction( allocator_type const& alloc );
};
///////////////////////////////////////////////////////////////////////////////
}

#include "SingleAllocator.inl"
