#pragma once
#include "core/macros.h"
#include "core/ptr/creation_payload.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class EntwinedCreator
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
			target->~T();
		}
		if( ref != nullptr )
		{
			ref->~PtrRef();
			free(ref);
		}
	}

	template<typename... U>
	static CreationPayload<T> Create(U&&... args)
	{
		// Only one allocation, with the ref serving as the root, since it will
		//  be the longest lived
		void * mem = malloc( sizeof(PtrRef) + sizeof(T) );
		PtrRef * newRef = new (mem) PtrRef(&Delete, nullptr);
		T * newT = new ((char*)mem+sizeof(PtrRef)) T(std::forward<U>(args)...);

		CreationPayload<T> retVal( newT, newRef );
		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}
