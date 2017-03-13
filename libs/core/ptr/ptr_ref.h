#pragma once
#include "core/macros.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class PtrRef
{
	RF_NO_COPY(PtrRef);

	//
	// Types
public:
	typedef uint32_t CountType;
	typedef void (*DeletionFunc)(T *, PtrRef *, void *);


	//
	// Public methods
public:
	PtrRef(DeletionFunc deletionFunc, void * funcUserData)
		: m_DeletionFunc(deletionFunc)
		, m_FuncUserData(funcUserData)
		, m_StrongCount(1)
		, m_WeakCount(1)
	{
		RF_ASSERT(m_DeletionFunc != nullptr);
	}

	void Delete( T * p ) const
	{
		RF_ASSERT(p != nullptr);
		RF_ASSERT(m_DeletionFunc != nullptr);
		m_DeletionFunc(p, nullptr, m_FuncUserData);
	}

	void DeleteSelf()
	{
		RF_ASSERT(this != nullptr);
		RF_ASSERT(m_DeletionFunc != nullptr);
		m_DeletionFunc(nullptr, this, m_FuncUserData);
	}

	CountType GetStrongCount() const
	{
		return m_StrongCount;
	}

	CountType GetWeakCount() const
	{
		return m_WeakCount;
	}

	void IncreaseStrongCount()
	{
		if( m_StrongCount > 0 )
		{
			m_StrongCount++;
		}
	}

	void DecreaseStrongCount(T * & target)
	{
		m_StrongCount--;
		if( m_StrongCount == 0 )
		{
			T * deletionTarget = target;
			target = nullptr;
			Delete(deletionTarget);
			DecreaseWeakCount();
		}
	}

	void IncreaseWeakCount()
	{
		m_WeakCount++;
	}

	void DecreaseWeakCount()
	{
		m_WeakCount--;
		if( m_WeakCount == 0 )
		{
			DeleteSelf();
		}
	}


	//
	// Private data
private:
	DeletionFunc m_DeletionFunc;
	void * m_FuncUserData;
	CountType m_StrongCount;
	CountType m_WeakCount;
};

///////////////////////////////////////////////////////////////////////////////
}
