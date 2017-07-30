#pragma once
#include "core/ptr/ptr_base.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class WeakPtr : public PtrBase<T>
{
	//
	// Types
public:
	typedef PtrBase<T> PtrBase;


	//
	// Public methods
public:
	WeakPtr()
		: PtrBase(nullptr, nullptr)
	{
		//
	}

	WeakPtr(std::nullptr_t)
		: WeakPtr()
	{
		//
	}

	WeakPtr( WeakPtr const & rhs )
		: WeakPtr(rhs.GetTarget(), rhs.GetRef())
	{
		//
	}

	WeakPtr( T * target, PtrRef * ref )
		: PtrBase(target, ref)
	{
		IncreaseWeakCount();
	}

	WeakPtr( WeakPtr && rhs )
		: PtrBase(std::move(rhs))
	{
		//
	}

	~WeakPtr()
	{
		DecreaseWeakCount();
	}

	WeakPtr & operator =(WeakPtr const & rhs)
	{
		WeakPtr temp(rhs);
		Swap( std::move(temp) );
		return *this;
	}

	WeakPtr & operator =(WeakPtr && rhs)
	{
		Swap( std::move(rhs) );
		return *this;
	}

	operator T *() const
	{
		return GetTargetAsWeak();
	}
};

///////////////////////////////////////////////////////////////////////////////
}
