#pragma once
#include "core/ptr/ptr_base.h"
#include "core/ptr/creation_payload.h"
#include "core/ptr/weak_ptr.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Unique strong reference to target, shares weak reference to target
template<typename T>
class UniquePtr : public PtrBase<T>
{
	RF_NO_COPY(UniquePtr);


	//
	// Types
public:
	typedef PtrBase<T> PtrBase;


	//
	// Public methods
public:
	UniquePtr()
		: PtrBase(nullptr, nullptr)
	{
		//
	}

	UniquePtr(std::nullptr_t)
		: UniquePtr()
	{
		//
	}

	UniquePtr( UniquePtr && rhs )
		: PtrBase(std::move(rhs))
	{
		//
	}

	UniquePtr( CreationPayload<T> && payload )
		: PtrBase(std::move(payload))
	{
		//
	}

	~UniquePtr()
	{
		DecreaseStrongCount();
	}

	UniquePtr & operator =(UniquePtr && rhs)
	{
		Swap( std::move(rhs) );
		return *this;
	}

	operator T *() const
	{
		return GetTarget();
	}

	T* operator ->() const
	{
		return GetTarget();
	}

	operator WeakPtr<T>() const
	{
		return WeakPtr<T>(GetTarget(), GetRef());
	}
};

///////////////////////////////////////////////////////////////////////////////
}
