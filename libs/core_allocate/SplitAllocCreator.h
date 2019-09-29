#pragma once
#include "core_allocate/Allocator.h"
#include "core/ptr/ptr_traits.h"
#include "core/ptr/creation_payload.h"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class SplitAllocCreator
{
	RF_NO_INSTANCE( SplitAllocCreator );

	//
	// Public methods
public:
	template<typename... U>
	static CreationPayload<T> Create( Allocator& allocator, U&&... args )
	{
		static_assert( sizeof( T ) + sizeof( PtrRef ) >= 64, "Should use entwined creator instead" );

		void* const memT = allocator.Allocate( sizeof( T ) );
		RF_ASSERT_MSG( memT != nullptr, "Failed to allocate" );
		void* const memRef = allocator.Allocate( sizeof( PtrRef ) );
		RF_ASSERT_MSG( memRef != nullptr, "Failed to allocate" );

		T* const newT = new( memT ) T( rftl::forward<U>( args )... );
		PtrRef* const newRef = new( memRef ) PtrRef( &Delete, &allocator );

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
			allocator.Delete( const_cast<void*>( target ) );
		}
		if( ref != nullptr )
		{
			ref->~PtrRef();
			allocator.Delete( ref );
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
}}
