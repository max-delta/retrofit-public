#pragma once
#include "core/ptr/ptr_base.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class WeakPtr : public PtrBase<T>
{
	RF_NO_COPY(WeakPtr);


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

	WeakPtr(nullptr_t)
		: WeakPtr()
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

	WeakPtr & operator =(WeakPtr && rhs)
	{
		Swap( std::move(rhs) );
		return *this;
	}
};

///////////////////////////////////////////////////////////////////////////////
}
