#pragma once
#include <memory>

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
	typedef void (*DeletionFunc)(T *);
	typedef void (*SelfDeletionFunc)(PtrRef *);


	//
	// Public methods
public:
	PtrRef(DeletionFunc deletionFunc, SelfDeletionFunc selfDeletionFunc)
		: m_DeletionFunc(deletionFunc)
		, m_SelfDeletionFunc(selfDeletionFunc)
		, m_StrongCount(1)
		, m_WeakCount(1)
	{
		RF_ASSERT(m_DeletionFunc != nullptr);
	}

	void Delete( T * p ) const
	{
		RF_ASSERT(p != nullptr);
		RF_ASSERT(m_DeletionFunc != nullptr);
		m_DeletionFunc(p);
	}

	CountType GetStrongCount() const
	{
		return m_StrongCount;
	}

	CountType GetWeakCount() const
	{
		return m_WeakCount;
	}

	void DecreaseStrongCount(T * target)
	{
		m_StrongCount--;
		if( m_StrongCount == 0 )
		{
			Delete(target);
			DecreaseWeakCount();
		}
	}

	void DecreaseWeakCount()
	{
		m_WeakCount--;
		if( m_WeakCount == 0 )
		{
			m_SelfDeletionFunc(this);
		}
	}


	//
	// Private data
private:
	DeletionFunc m_DeletionFunc;
	SelfDeletionFunc m_SelfDeletionFunc;
	CountType m_StrongCount;
	CountType m_WeakCount;
};

///////////////////////////////////////////////////////////////////////////////
}
