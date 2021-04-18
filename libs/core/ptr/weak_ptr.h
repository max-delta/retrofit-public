#pragma once
#include "core/ptr/ptr_base.h"
#include "rftl/cstddef"


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
	using PtrBase = PtrBase<T>;


	//
	// Public methods
public:
	WeakPtr()
		: PtrBase( nullptr, nullptr )
	{
		//
	}

	WeakPtr( rftl::nullptr_t )
		: WeakPtr()
	{
		//
	}

	// T -> T
	WeakPtr( WeakPtr const& rhs )
		: WeakPtr( rhs.GetTarget(), rhs.GetRef() )
	{
		//
	}

	// T -> T const
	// Enable only if T is a const version of NONCONST
	template<typename NONCONST, typename rftl::enable_if<rftl::is_same<typename rftl::add_const<NONCONST>::type, T>::value, int>::type = 0>
	WeakPtr( WeakPtr<NONCONST> const& rhs )
		: WeakPtr( rhs.GetTarget(), rhs.GetRef() )
	{
		//
	}

	// T -> DERIVED
	// Enable only if DERIVED is not a CV variation of T
	template<typename DERIVED, typename rftl::enable_if<rftl::is_same<typename rftl::remove_cv<DERIVED>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	WeakPtr( WeakPtr<DERIVED> const& rhs )
		: WeakPtr( rhs.GetTarget(), rhs.GetRef() )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
	}

	// T -> T
	WeakPtr( T* target, PtrRef* ref )
		: PtrBase( target, ref )
	{
		PtrBase::IncreaseWeakCount();
	}

	// T -> DERIVED
	// Enable only if DERIVED is not a CV variation of T
	template<typename DERIVED, typename rftl::enable_if<rftl::is_same<typename rftl::remove_cv<DERIVED>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	WeakPtr( DERIVED* target, PtrRef* ref )
		: PtrBase( target, ref )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		PtrBase::IncreaseWeakCount();
	}

	WeakPtr( WeakPtr&& rhs )
		: PtrBase( rftl::move( rhs ) )
	{
		//
	}

	~WeakPtr()
	{
		PtrBase::DecreaseWeakCount();
	}

	WeakPtr& operator=( WeakPtr const& rhs )
	{
		WeakPtr temp( rhs );
		PtrBase::Swap( rftl::move( temp ) );
		return *this;
	}

	WeakPtr& operator=( WeakPtr&& rhs )
	{
		PtrBase::Swap( rftl::move( rhs ) );
		return *this;
	}

	T* Get() const
	{
		return static_cast<T*>( *this );
	}

	operator T*() const
	{
		return PtrBase::GetTargetAsWeak();
	}

	T* operator->() const
	{
		return PtrBase::GetTargetAsWeak();
	}
};

///////////////////////////////////////////////////////////////////////////////
}
