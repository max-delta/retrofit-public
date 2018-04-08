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
	typedef PtrBase<T> PtrBase;
	typedef PtrRef<T> PtrRef;


	//
	// Public methods
public:
	WeakPtr()
		: PtrBase(nullptr, nullptr)
	{
		//
	}

	WeakPtr( rftl::nullptr_t)
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
		PtrBase::IncreaseWeakCount();
	}

	template<typename DERIVED, typename PTRREFDERIVED>
	WeakPtr( DERIVED * target, PTRREFDERIVED * ref )
		: PtrBase(target, ref)
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		static_assert( rftl::is_same<PTRREFDERIVED, RF::PtrRef<DERIVED> >::value, "Expected to receive DERIVED* and PtrRef<DERIVED>*" );
		PtrBase::IncreaseWeakCount();
	}

	WeakPtr( WeakPtr && rhs )
		: PtrBase( rftl::move( rhs ) )
	{
		//
	}

	~WeakPtr()
	{
		PtrBase::DecreaseWeakCount();
	}

	WeakPtr & operator =( WeakPtr const & rhs )
	{
		WeakPtr temp( rhs );
		PtrBase::Swap( rftl::move( temp ) );
		return *this;
	}

	WeakPtr & operator =( WeakPtr && rhs )
	{
		PtrBase::Swap( rftl::move( rhs ) );
		return *this;
	}

	T * Get() const
	{
		return (T*)( *this );
	}

	operator T *() const
	{
		return PtrBase::GetTargetAsWeak();
	}

	T* operator ->() const
	{
		return PtrBase::GetTargetAsWeak();
	}
};

///////////////////////////////////////////////////////////////////////////////
}
