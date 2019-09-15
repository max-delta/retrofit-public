#pragma once
#include "core_allocate/AlignmentHelpers.h"

#include "core/meta/ConstructorOverload.h"

#include "rftl/atomic"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

template<size_t MaxTotalSize, size_t Align = kMaxAlign>
class LinearAllocator
{
	RF_NO_COPY( LinearAllocator );
	RF_NO_MOVE( LinearAllocator );

	//
	// Types and constants
public:
	using ThisType = LinearAllocator<MaxTotalSize>;
	static constexpr size_t kMaxTotalSize = MaxTotalSize;
	static constexpr size_t kAlignment = Align;
	static_assert( IsValidAlignment( kAlignment ), "Invalid alignment" );


	//
	// Public methods
public:
	// No implicit default construction, to prevent accidental creation
	LinearAllocator() = delete;
	explicit LinearAllocator( ExplicitDefaultConstruct );

	void* Allocate( size_t size );
	void* Allocate( size_t size, size_t align );
	void Delete( void* ptr );
	size_t GetMaxSize() const;
	size_t GetCurrentSize() const;
	size_t GetCurrentCount() const;


	//
	// Private data
private:
	rftl::atomic<size_t> mBytesAllocated = 0;
	rftl::atomic<size_t> mCurrentAllocations = 0;

	// Storage will be aligned as requested, which will not change the size of
	//  the storage, but may require some padding before-hand to fit properly
	RF_ACK_ANY_PADDING;
	alignas( kAlignment ) uint8_t mStorageBytes[kMaxTotalSize];
	static_assert( sizeof( mStorageBytes ) == kMaxTotalSize, "Unexpected size" );
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "LinearAllocator.inl"
