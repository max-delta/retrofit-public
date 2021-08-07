#pragma once
#include "Allocation/AccessorDeclaration.h"
#include "Allocation/AllocatorRegistry.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// Non-inlined, to keep the main cache function from pulling in a lot of weight
inline RF_NO_INLINE Allocator* GetAllocator( char const* identifier )
{
	return GetOrCreateGlobalAllocatorRegistry().GetAllocator( identifier );
}

}
///////////////////////////////////////////////////////////////////////////////

template<typename TagT>
Allocator* GetAllocator()
{
	// Tags must have a constexpr null-terminated string identifier
	// NOTE: This should prevent COMDAT folding for cases where the identifier
	//  is not itself COMDAT-folded
	static constexpr char const* kTagIdentifier = TagT::kIdentifier;

	// Fetch and cache the allocator
	// NOTE: Allocator is assumed to be extremely long-lived, otherwise memory
	//  corruption will likely occur
	// NOTE: The function-local static here should prevent COMDAT folding for
	//  everything but the most aggressive optimizations (Is there a condition
	//  where any existing toolchain will still fold this?)
	static Allocator* allocatorCache = details::GetAllocator( kTagIdentifier );

	return allocatorCache;
}

///////////////////////////////////////////////////////////////////////////////
}
