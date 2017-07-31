#pragma once
#include "core/ptr/ptr_base.h"
#include "core/ptr/creation_payload.h"
#include "core/ptr/weak_ptr.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Shares strong reference to target, shares weak reference to target
template<typename T>
class SharedPtr : public PtrBase<T>
{
	//
	// Types
public:
	typedef PtrBase<T> PtrBase;


	//
	// Public methods
public:
	SharedPtr()
		: PtrBase(nullptr, nullptr)
	{
		//
	}

	SharedPtr(std::nullptr_t)
		: SharedPtr()
	{
		//
	}

	SharedPtr( SharedPtr const & rhs )
		: PtrBase(rhs.GetTarget(), rhs.GetRef())
	{
		IncreaseStrongCount();
	}

	template<typename DERIVED>
	SharedPtr( SharedPtr<DERIVED> const & rhs )
		: PtrBase(rhs.GetTarget(), rhs.GetRef())
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		IncreaseStrongCount();
	}

	SharedPtr( SharedPtr && rhs )
		: PtrBase(std::move(rhs))
	{
		//
	}

	SharedPtr( CreationPayload<T> && payload )
		: PtrBase(std::move(payload))
	{
		//
	}

	template<typename DERIVED>
	SharedPtr( CreationPayload<DERIVED> && payload )
		: PtrBase(std::move(payload))
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
	}

	~SharedPtr()
	{
		DecreaseStrongCount();
	}

	SharedPtr & operator =(SharedPtr const & rhs)
	{
		SharedPtr temp(rhs);
		Swap( std::move(temp) );
		return *this;
	}

	SharedPtr & operator =(SharedPtr && rhs)
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

	template<typename BASE>
	operator WeakPtr<BASE>() const
	{
		RF_PTR_ASSERT_CASTABLE( BASE, T );
		return WeakPtr<BASE>(GetTarget(), GetRef());
	}
};

///////////////////////////////////////////////////////////////////////////////
}
