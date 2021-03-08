#pragma once
#include "core_allocate/Allocator.h"
#include "core/ptr/ptr_traits.h"
#include "core/ptr/creation_payload.h"


namespace RF::alloc {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class EntwinedAllocCreator
{
	RF_NO_INSTANCE( EntwinedAllocCreator );

	//
	// Public methods
public:
	template<typename... U>
	static CreationPayload<T> Create( Allocator& allocator, U&&... args )
	{
		// Only one allocation, with the ref serving as the root, since it will
		//  be the longest lived
		static_assert( sizeof( T ) + sizeof( PtrRef ) <= 128, "Should not use entwined creator" );

		void* const mem = allocator.Allocate( sizeof( PtrRef ) + sizeof( T ) );
		RF_ASSERT_MSG( mem != nullptr, "Failed to allocate" );

		PtrRef* const newRef = new( mem ) PtrRef( &Delete, &allocator );
		T* const newT = new( reinterpret_cast<char*>( mem ) + sizeof( PtrRef ) ) T( rftl::forward<U>( args )... );

		CreationPayload<T> retVal( newT, newRef );
		return retVal;
	}


	//
	// Private methods
private:
	static void Delete( void const* target, PtrRef* ref, void* userData )
	{
		RF_PTR_ASSERT_DELETABLE( T );

		Allocator& allocator = *reinterpret_cast<Allocator*>( userData );
		if( target != nullptr )
		{
			reinterpret_cast<T const*>( target )->~T();
		}
		if( ref != nullptr )
		{
			ref->~PtrRef();
			allocator.Delete( ref );
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
}
