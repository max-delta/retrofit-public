#pragma once
#include "core_allocate/Allocator.h"
#include "core_allocate/LinearAllocator.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

// Uses a linear allocator as much as it can, before falling back to a generic
//  allocator via virtual interface
// NOTE: Intended to be used for stack allocations that will fall back to heap
template<size_t LinearSize, size_t Align = kMaxAlign>
class LinearStretchAllocator
{
	RF_NO_COPY( LinearStretchAllocator );
	RF_NO_MOVE( LinearStretchAllocator );

	//
	// Types and constants
public:
	using ThisType = LinearStretchAllocator<LinearSize, Align>;
	static constexpr size_t kLinearSize = LinearSize;
	static constexpr size_t kAlignment = Align;
	static_assert( IsValidAlignment( kAlignment ), "Invalid alignment" );
private:
	using TrueLinearAllocator = LinearAllocator<kLinearSize, kAlignment>;


	//
	// Public methods
public:
	// No implicit default construction, to prevent accidental creation
	LinearStretchAllocator() = delete;
	explicit LinearStretchAllocator( Allocator& stretchAllocator );
	~LinearStretchAllocator();

	void* Allocate( size_t size );
	void* Allocate( size_t size, size_t align );
	void Delete( void* ptr );
	size_t GetMaxSize() const;
	size_t GetCurrentSize() const;
	size_t GetCurrentCount() const;
	void RelinquishAllAllocations();

	size_t GetCurrentStretchCount() const;


	//
	// Private data
private:
	// Will try to use a linear allocator up until it fails, after which will
	//  switch to stretch usage instead
	TrueLinearAllocator mLinearAllocator;
	Allocator& mStretchAllocator;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "LinearStretchAllocator.inl"
