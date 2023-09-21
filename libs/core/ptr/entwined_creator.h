#pragma once
#include "core/macros.h"
#include "core/ptr/ptr_traits.h"
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

		void* const mem = ::operator new( sizeof( PtrRef ) + sizeof( T ) );

		PtrRef* const newRef = new( mem ) PtrRef( &Delete, nullptr );
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

		(void)userData;
		if( target != nullptr )
		{
			// NOTE: Clang suppression here is for the check that is already
			//  done above by RF_PTR_ASSERT_DELETABLE(...), which supports a
			//  mechanism to bypass it for special cases
			RF_CLANG_PUSH();
			RF_CLANG_IGNORE( "-Wdelete-non-abstract-non-virtual-dtor" );
			// NOTE: Subtle... making sure we delete the actual pointer type
			reinterpret_cast<T const*>( target )->~T();
			RF_CLANG_POP();
		}
		if( ref != nullptr )
		{
			ref->~PtrRef();
			::operator delete( ref );
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
}
