#include "stdafx.h"
#include "AllocatorRegistry.h"

#include "Allocation/HeapAllocator.h"

#include "core/ptr/default_creator.h"
#include "core/meta/LazyInitSingleton.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

AllocatorRegistry::AllocatorRegistry()
	: mFallbackAllocator( DefaultCreator<AllocatorT<HeapAllocator>>::Create( ExplicitDefaultConstruct() ) )
{
	//
}



WeakPtr<Allocator> AllocatorRegistry::GetAllocator( AllocatorIdentifier const& identifier ) const
{
	AllocatorLookup::const_iterator const iter = mAllocatorLookup.find( identifier );
	if( iter == mAllocatorLookup.end() )
	{
		static constexpr bool kAlertOnFallback = false;
		if( kAlertOnFallback )
		{
			RF_DBGFAIL_MSG( "Failed to find allocator" );
		}
		return mFallbackAllocator;
	}
	return iter->second;
}



WeakPtr<Allocator> AllocatorRegistry::AddAllocator( AllocatorIdentifier const& identifier, UniquePtr<Allocator>&& allocator )
{
	rftl::pair<AllocatorLookup::iterator, bool> const emplaceResult =
		mAllocatorLookup.emplace( identifier, rftl::move( allocator ) );
	RF_ASSERT_MSG( emplaceResult.second, "Allocator already registered" );
	RF_ASSERT( emplaceResult.first->second );
	return emplaceResult.first->second;
}



UniquePtr<Allocator> AllocatorRegistry::RemoveAllocator( AllocatorIdentifier const& identifier )
{
	AllocatorLookup::iterator const iter = mAllocatorLookup.find( identifier );
	RF_ASSERT_MSG( iter != mAllocatorLookup.end(), "Failed to find allocator" );
	UniquePtr<Allocator> retVal = rftl::move( iter->second );
	mAllocatorLookup.erase( iter );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

AllocatorRegistry& GetOrCreateGlobalAllocatorRegistry()
{
	return GetOrInitFunctionStaticScopedSingleton<AllocatorRegistry>();
}

///////////////////////////////////////////////////////////////////////////////
}
