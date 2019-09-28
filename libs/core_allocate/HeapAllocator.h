#pragma once
#include "core/meta/ConstructorOverload.h"
#include "core/macros.h"

#include "rftl/atomic"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

class HeapAllocator
{
	RF_NO_COPY( HeapAllocator );
	RF_NO_MOVE( HeapAllocator );

	//
	// Public methods
public:
	// No implicit default construction, to prevent accidental creation
	HeapAllocator() = delete;
	explicit HeapAllocator( ExplicitDefaultConstruct );
	~HeapAllocator();

	void* Allocate( size_t size );
	void* Allocate( size_t size, size_t align );
	void Delete( void* ptr );
	size_t GetMaxSize() const;
	size_t GetCurrentSize() const;
	size_t GetCurrentCount() const;

	void RelinquishAllAllocations();


	//
	// Private data
private:
	void const* const mHeapHandle = nullptr;
	rftl::atomic<size_t> mAllocCount = 0;
	rftl::atomic<bool> mHasRelinquishedAllAllocations = false;
};

///////////////////////////////////////////////////////////////////////////////
}}
