#pragma once
#include "core/meta/ConstructorOverload.h"

#include "rftl/atomic"
#include "rftl/memory"
#include "rftl/type_traits"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

template<size_t MaxTotalSize, size_t Align = 1>
class SingleAllocator
{
	RF_NO_COPY( SingleAllocator );
	RF_NO_MOVE( SingleAllocator );

	//
	// Types and constants
public:
	using ThisType = SingleAllocator<MaxTotalSize>;
	static constexpr size_t kMaxTotalSize = MaxTotalSize;


	//
	// Public methods
public:
	// No implicit default construction, to prevent accidental creation
	SingleAllocator() = delete;
	explicit SingleAllocator( ExplicitDefaultConstruct );

	//template<typename U, size_t OtherMaxTotalSize>
	//SingleAllocator( SingleAllocator<U, OtherMaxTotalSize> const& rhs ) = delete;
	//template<typename U, size_t OtherMaxTotalSize>
	//SingleAllocator( SingleAllocator<U, OtherMaxTotalSize>&& rhs ) = delete;

	void* Allocate( size_t size );
	void* Allocate( size_t size, size_t align );
	void Delete( void* ptr );
	size_t GetMaxSize() const;


	//
	// Private data
private:
	volatile rftl::atomic_bool mHasAllocation;
	alignas( Align ) uint8_t mStorageBytes[kMaxTotalSize];
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "SingleAllocator.inl"
