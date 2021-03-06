#pragma once
#include "core/macros.h"
#include "core/ptr/ptr_traits.h"
#include "core/ptr/creation_payload.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class SplitCreator
{
	RF_NO_INSTANCE( SplitCreator );

	//
	// Public methods
public:
	template<typename... U>
	static CreationPayload<T> Create( U&&... args )
	{
		static_assert( sizeof( T ) + sizeof( PtrRef ) >= 64, "Should use entwined creator instead" );
		CreationPayload<T> retVal(
			new T( rftl::forward<U>( args )... ),
			new PtrRef( &Delete, nullptr ) );
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
			// NOTE: Subtle... making sure we delete the actual pointer type
			delete( reinterpret_cast<T const*>( target ) );
		}
		if( ref != nullptr )
		{
			delete ref;
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
}
