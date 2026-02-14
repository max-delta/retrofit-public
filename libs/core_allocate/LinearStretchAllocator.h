#pragma once
#include "core_allocate/LinearAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

template<size_t NonHeapSize, size_t Align = kMaxAlign>
class LinearStretchAllocator
{
	RF_NO_COPY( LinearStretchAllocator );
	RF_NO_MOVE( LinearStretchAllocator );

	//
	// Types and constants
public:
	using ThisType = LinearStretchAllocator<NonHeapSize, Align>;
	static constexpr size_t kNonHeapSize = NonHeapSize;
	static constexpr size_t kAlignment = Align;
	static_assert( IsValidAlignment( kAlignment ), "Invalid alignment" );
private:
	using TrueLinearAllocator = LinearAllocator<NonHeapSize, Align>;


	//
	// Public methods
public:
	// No implicit default construction, to prevent accidental creation
	LinearStretchAllocator() = delete;
	explicit LinearStretchAllocator( ExplicitDefaultConstruct );
	~LinearStretchAllocator();

	void* Allocate( size_t size );
	void* Allocate( size_t size, size_t align );
	void Delete( void* ptr );
	size_t GetCurrentHeapCount() const;


	//
	// Private data
private:
	// Will try to use a linear allocator up until it fails, after which will
	//  switch to heap usage instead
	TrueLinearAllocator mLinearAllocator;
	rftl::atomic<size_t> mHeapAllocations = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "LinearStretchAllocator.inl"
