#pragma once
#include "core/ptr/ptr_base.h"
#include "core/ptr/creation_payload.h"
#include "core/ptr/weak_ptr.h"

#include "rftl/type_traits"
#include "rftl/cstdlib"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Unique strong reference to target, shares weak reference to target
template<typename T>
class UniquePtr : public PtrBase<T>
{
	RF_NO_COPY( UniquePtr );


	//
	// Friends
private:
	template<typename PEER> friend class UniquePtr;


	//
	// Types
public:
	using PtrBase = PtrBase<T>;


	//
	// Public methods
public:
	UniquePtr()
		: PtrBase( nullptr, nullptr )
	{
		//
	}

	UniquePtr( rftl::nullptr_t )
		: UniquePtr()
	{
		//
	}

	// T -> T
	UniquePtr( UniquePtr&& rhs )
		: PtrBase( rftl::move( rhs ) )
	{
		//
	}

	// T -> T const
	// Enable only if T is a const version of NONCONST
	template<typename NONCONST, typename rftl::enable_if<rftl::is_same<typename rftl::add_const<NONCONST>::type, T>::value, int>::type = 0>
	UniquePtr( UniquePtr<NONCONST>&& rhs )
		: PtrBase( rftl::move( rhs.CreateTransferPayloadAndWipeSelf() ) )
	{
		RF_ASSERT( rhs == nullptr );
	}

	// T -> DERIVED
	// Enable only if DERIVED is not a CV variation of T
	template<typename DERIVED, typename rftl::enable_if<rftl::is_same<typename rftl::remove_cv<DERIVED>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	UniquePtr( UniquePtr<DERIVED>&& rhs )
		: PtrBase( rftl::move( rhs.CreateTransferPayloadAndWipeSelf() ) )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		RF_ASSERT( rhs == nullptr );
	}

	UniquePtr( CreationPayload<T>&& payload )
		: PtrBase( rftl::move( payload ) )
	{
		if( PtrBase::IsUniqueStrongOwner() == false )
		{
			RF_DBGFAIL_MSG( "UniquePtr is not unique after creation" );
			rftl::abort();
		}
	}

	template<typename DERIVED>
	UniquePtr( CreationPayload<DERIVED>&& payload )
		: PtrBase( rftl::move( payload ) )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );

		if( PtrBase::IsUniqueStrongOwner() == false )
		{
			RF_DBGFAIL_MSG( "UniquePtr is not unique after creation" );
			rftl::abort();
		}
	}

	~UniquePtr()
	{
		if( PtrBase::IsUniqueStrongOwner() == false )
		{
			RF_DBGFAIL_MSG( "UniquePtr is not unique while destructing" );
			rftl::abort();
		}

		PtrBase::DecreaseStrongCount();
	}

	// T -> T
	UniquePtr& operator=( UniquePtr&& rhs )
	{
		PtrBase::Swap( rftl::move( rhs ) );
		UniquePtr discard = {};
		discard.Swap( rftl::move( rhs ) );
		return *this;
	}

	T* Get() const
	{
		return static_cast<T*>( *this );
	}

	operator T*() const
	{
		return PtrBase::GetTarget();
	}

	T* operator->() const
	{
		return PtrBase::GetTarget();
	}

	operator WeakPtr<T>() const
	{
		return WeakPtr<T>( PtrBase::GetTarget(), PtrBase::GetRef() );
	}

	template<typename CONST, typename rftl::enable_if<
		rftl::is_const<CONST>::value &&
		rftl::is_same<typename rftl::remove_const<CONST>::type, T>::value, int>::type = 0>
	operator WeakPtr<CONST>() const
	{
		return WeakPtr<CONST>( PtrBase::GetTarget(), PtrBase::GetRef() );
	}

	template<typename BASE, typename rftl::enable_if<
		rftl::is_same<typename rftl::remove_cv<BASE>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	operator WeakPtr<BASE>() const
	{
		RF_PTR_ASSERT_CASTABLE( BASE, T );
		return WeakPtr<BASE>( PtrBase::GetTarget(), PtrBase::GetRef() );
	}
};

///////////////////////////////////////////////////////////////////////////////
}
