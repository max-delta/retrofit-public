#pragma once
#include "core/ptr/shared_ptr.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class WeakSharedPtr : public PtrBase<T>
{
	//
	// Types
public:
	typedef PtrBase<T> PtrBase;


	//
	// Public methods
public:
	WeakSharedPtr()
		: PtrBase(nullptr, nullptr)
	{
		//
	}

	WeakSharedPtr(nullptr_t)
		: WeakSharedPtr()
	{
		//
	}

	WeakSharedPtr( WeakSharedPtr const & rhs )
		: WeakSharedPtr(rhs.GetTarget(), rhs.GetRef())
	{
		//
	}

	WeakSharedPtr( T * target, PtrRef * ref )
		: PtrBase(target, ref)
	{
		IncreaseWeakCount();
	}

	WeakSharedPtr( WeakSharedPtr && rhs )
		: PtrBase(std::move(rhs))
	{
		//
	}

	WeakSharedPtr( SharedPtr<T> const & rhs )
		: WeakSharedPtr(rhs.GetTarget(), rhs.GetRef())
	{
		//
	}

	~WeakSharedPtr()
	{
		DecreaseWeakCount();
	}

	WeakSharedPtr & operator =(WeakSharedPtr const & rhs)
	{
		WeakSharedPtr temp(rhs);
		Swap( std::move(temp) );
		return *this;
	}

	WeakSharedPtr & operator =(WeakSharedPtr && rhs)
	{
		Swap( std::move(rhs) );
		return *this;
	}

	operator WeakPtr<T>() const
	{
		return WeakPtr<T>(GetTarget(), GetRef());
	}

	SharedPtr<T> Lock()
	{
		IncreaseStrongCount();
		return CreateTransferPayload();
	}
};

///////////////////////////////////////////////////////////////////////////////
}
