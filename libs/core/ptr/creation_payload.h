#pragma once
#include "core/macros.h"
#include "core/ptr/ptr_ref.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class CreationPayload
{
	RF_NO_COPY(CreationPayload);


	//
	// Friends
private:
	template<typename T> friend class PtrBase;
	template<typename T> friend struct PtrTransformer;


	//
	// Types
public:
	typedef PtrRef<T> PtrRef;


	//
	// Public methods
public:
	CreationPayload( T * target, PtrRef * ref )
		: m_Target(target)
		, m_Ref(ref)
	{
		//
	}

	CreationPayload( CreationPayload && rhs )
		: m_Target( rftl::move( rhs.m_Target ) )
		, m_Ref( rftl::move( rhs.m_Ref ) )
	{
		rhs.Clean();
	}

	~CreationPayload()
	{
		RF_ASSERT(m_Target == nullptr);
		RF_ASSERT(m_Ref == nullptr);
		if( m_Ref != nullptr )
		{
			// Someone dropped the ball during transfer!
			m_Ref->Delete(m_Target);
			delete m_Ref;
		}
	}


	//
	// Private methods
private:
	void Clean()
	{
		m_Target = nullptr;
		m_Ref = nullptr;
	}


	//
	// Private data
private:
	T * m_Target;
	PtrRef * m_Ref;
};

///////////////////////////////////////////////////////////////////////////////
}
