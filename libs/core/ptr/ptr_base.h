#pragma once
#include "core/ptr/ptr_ref.h"
#include "core/ptr/creation_payload.h"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class PtrBase
{
	//
	// Types
public:
	typedef PtrRef<T> PtrRef;


	//
	// Public methods
public:
	PtrBase( CreationPayload<T> && payload )
		: m_Target(payload.m_Target)
		, m_Ref(payload.m_Ref)
	{
		payload.Clean();
	}


	//
	// Protected methods
public:
	void DecreaseStrongCount()
	{
		if( m_Ref != nullptr )
		{
			m_Ref->DecreaseStrongCount(m_Target);
		}
	}


	//
	// Private data
private:
	T * m_Target;
	PtrRef * m_Ref;
};

///////////////////////////////////////////////////////////////////////////////
}
