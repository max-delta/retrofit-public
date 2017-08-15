#pragma once
#include "core/ptr/ptr_base.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Shares weak reference to target
template<typename T>
class WeakPtr : public PtrBase<T>
{
	//
	// Friends
private:
	template<typename PEER> friend class WeakPtr;


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

	template<typename DERIVED>
	WeakPtr( WeakPtr<DERIVED> const & rhs )
		: WeakPtr(rhs.GetTarget(), rhs.GetRef())
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
	}

	WeakPtr( T * target, PtrRef * ref )
		: PtrBase(target, ref)
	{
		IncreaseWeakCount();
	}

	template<typename DERIVED, typename PTRREFDERIVED>
	WeakPtr( DERIVED * target, PTRREFDERIVED * ref )
		: PtrBase(target, ref)
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		static_assert( std::is_same<PTRREFDERIVED, RF::PtrRef<DERIVED> >::value, "Expected to receive DERIVED* and PtrRef<DERIVED>*" );
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

	T* operator ->() const
	{
		return GetTargetAsWeak();
	}
};

///////////////////////////////////////////////////////////////////////////////
}
