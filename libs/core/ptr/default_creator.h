#pragma once
#include "core/macros.h"
#include "core/ptr/creation_payload.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class DefaultCreator
{
	//
	// Types
public:
	typedef PtrRef<T> PtrRef;


	//
	// Public methods
public:
	static void Delete( T * target, PtrRef * ref, void * userData )
	{
		(void)userData;
		if( target != nullptr )
		{
			delete target;
		}
		if( ref != nullptr )
		{
			delete ref;
		}
	}

	template<typename... U>
	static CreationPayload<T> Create(U&&... args)
	{
		static_assert( sizeof( T ) + sizeof( PtrRef ) > 64, "Should use entwined creator instead" );
		CreationPayload<T> retVal(
			new T(std::forward<U>(args)...),
			new PtrRef(&Delete, nullptr) );
		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}
