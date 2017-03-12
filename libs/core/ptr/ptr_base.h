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
	explicit PtrBase( T * target, PtrRef * ref )
		: m_Target(target)
		, m_Ref(ref)
	{
		//
	}

	explicit PtrBase( PtrBase && rhs )
	{
		Swap( std::move(rhs) );
	}

	void DecreaseStrongCount()
	{
		if( m_Ref != nullptr )
		{
			m_Ref->DecreaseStrongCount(m_Target);
		}
	}

	void IncreaseWeakCount()
	{
		if( m_Ref != nullptr )
		{
			m_Ref->IncreaseWeakCount();
		}
	}

	void DecreaseWeakCount()
	{
		if( m_Ref != nullptr )
		{
			m_Ref->DecreaseWeakCount();
		}
	}

	void Swap( PtrBase && rhs )
	{
		std::swap( m_Ref, rhs.m_Ref );
		std::swap( m_Target, rhs.m_Target );
	}

	T * GetTarget() const
	{
		return m_Target;
	}

	PtrRef * GetRef() const
	{
		return m_Ref;
	}


	//
	// Private data
private:
	T * m_Target;
	PtrRef * m_Ref;
};

///////////////////////////////////////////////////////////////////////////////
}
