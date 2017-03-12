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
	static void Delete( T * p )
	{
		delete p;
	}

	static void DeleteRef( PtrRef * p )
	{
		delete p;
	}

	template<typename... U>
	static CreationPayload<T> Create(U&&... args)
	{
		CreationPayload<T> retVal(
			new T(std::forward<U>(args)...),
			new PtrRef(&Delete, &DeleteRef) );
		return retVal;
	}


	//
	// Private data
private:
	T * m_Target;
	PtrRef * m_Ref;
};

///////////////////////////////////////////////////////////////////////////////
}