#pragma once
#include "core/macros.h"
#include "core/ptr/creation_payload.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class EntwinedCreator
{
	RF_NO_INSTANCE( EntwinedCreator );

	//
	// Public methods
public:
	template<typename... U>
	static CreationPayload<T> Create( U&&... args )
	{
		// Only one allocation, with the ref serving as the root, since it will
		//  be the longest lived
		static_assert( sizeof( T ) + sizeof( PtrRef ) <= 128, "Should not use entwined creator" );
		void* mem = malloc( sizeof( PtrRef ) + sizeof( T ) );
		PtrRef* newRef = new( mem ) PtrRef( &Delete, nullptr );
		T* newT = new( reinterpret_cast<char*>( mem ) + sizeof( PtrRef ) ) T( rftl::forward<U>( args )... );

		CreationPayload<T> retVal( newT, newRef );
		return retVal;
	}


	//
	// Private methods
private:
	static void Delete( void* target, PtrRef* ref, void* userData )
	{
		(void)userData;
		if( target != nullptr )
		{
			reinterpret_cast<T*>( target )->~T();
		}
		if( ref != nullptr )
		{
			ref->~PtrRef();
			free( ref );
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
}
