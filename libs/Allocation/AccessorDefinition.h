#pragma once
#include "Allocation/AccessorDeclaration.h"
#include "Allocation/AllocatorRegistry.h"


namespace RF { namespace alloc {
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
	static Allocator* allocatorCache = GetOrCreateGlobalAllocatorRegistry().GetAllocator( kTagIdentifier );

	return allocatorCache;
}

///////////////////////////////////////////////////////////////////////////////
}}
