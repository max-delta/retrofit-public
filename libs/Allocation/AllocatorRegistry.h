#pragma once
#include "project.h"

#include "core/meta/ConstructorOverload.h"
#include "core/ptr/unique_ptr.h"

#include "rftl/string"
#include "rftl/unordered_map"

// Forwards
namespace RF { namespace alloc {
class Allocator;
}}


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

class ALLOCATION_API AllocatorRegistry
{
	RF_NO_COPY( AllocatorRegistry );
	RF_NO_MOVE( AllocatorRegistry );

	//
	// Types and constants
private:
	using AllocatorIdentifier = rftl::string;
	using AllocatorLookup = rftl::unordered_map<AllocatorIdentifier, UniquePtr<Allocator>>;

	//
	// Public methods
public:
	AllocatorRegistry() = default;

	WeakPtr<Allocator> GetAllocator( AllocatorIdentifier const& identifier ) const;

	WeakPtr<Allocator> AddAllocator( AllocatorIdentifier const& identifier, UniquePtr<Allocator>&& allocator );
	UniquePtr<Allocator> RemoveAllocator( AllocatorIdentifier const& identifier );


	//
	// Private data
private:
	AllocatorLookup mAllocatorLookup;
};

///////////////////////////////////////////////////////////////////////////////

ALLOCATION_API AllocatorRegistry& GetOrCreateGlobalAllocatorRegistry();

///////////////////////////////////////////////////////////////////////////////
}}
