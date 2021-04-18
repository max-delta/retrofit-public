#pragma once
#include "core/ptr/ptr_base.h"
#include "core/ptr/creation_payload.h"
#include "core/ptr/weak_ptr.h"

#include "rftl/type_traits"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Shares strong reference to target, shares weak reference to target
template<typename T>
class SharedPtr : public PtrBase<T>
{
	//
	// Friends
private:
	template<typename PEER> friend class SharedPtr;
	template<typename LIASON> friend class WeakSharedPtr;


	//
	// Types
public:
	using PtrBase = PtrBase<T>;


	//
	// Public methods
public:
	SharedPtr()
		: PtrBase( nullptr, nullptr )
	{
		//
	}

	SharedPtr( rftl::nullptr_t )
		: SharedPtr()
	{
		//
	}

	// T -> T
	SharedPtr( SharedPtr const& rhs )
		: PtrBase( rhs.GetTarget(), rhs.GetRef() )
	{
		PtrBase::IncreaseStrongCount();
	}

	// T -> T const
	// Enable only if T is a const version of NONCONST
	template<typename NONCONST, typename rftl::enable_if<rftl::is_same<typename rftl::add_const<NONCONST>::type, T>::value, int>::type = 0>
	SharedPtr( SharedPtr<NONCONST> const& rhs )
		: PtrBase( rhs.GetTarget(), rhs.GetRef() )
	{
		PtrBase::IncreaseStrongCount();
	}

	// T -> DERIVED
	// Enable only if DERIVED is not a CV variation of T
	template<typename DERIVED, typename rftl::enable_if<rftl::is_same<typename rftl::remove_cv<DERIVED>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	SharedPtr( SharedPtr<DERIVED> const& rhs )
		: PtrBase( rhs.GetTarget(), rhs.GetRef() )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		PtrBase::IncreaseStrongCount();
	}

	// T -> T
	SharedPtr( SharedPtr&& rhs )
		: PtrBase( rftl::move( rhs ) )
	{
		//
	}

	// T -> T const
	// Enable only if T is a const version of NONCONST
	template<typename NONCONST, typename rftl::enable_if<rftl::is_same<typename rftl::add_const<NONCONST>::type, T>::value, int>::type = 0>
	SharedPtr( SharedPtr<NONCONST>&& rhs )
		: PtrBase( rftl::move( rhs ) )
	{
		//
	}

	// T -> DERIVED
	// Enable only if DERIVED is not a CV variation of T
	template<typename DERIVED, typename rftl::enable_if<rftl::is_same<typename rftl::remove_cv<DERIVED>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	SharedPtr( SharedPtr<DERIVED>&& rhs )
		: PtrBase( rftl::move( rhs.CreateTransferPayloadAndWipeSelf() ) )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		RF_ASSERT( rhs == nullptr );
	}

	// T -> T
	SharedPtr( CreationPayload<T>&& payload )
		: PtrBase( rftl::move( payload ) )
	{
		//
	}

	// T -> T const
	// Enable only if T is a const version of NONCONST
	template<typename NONCONST, typename rftl::enable_if<rftl::is_same<typename rftl::add_const<NONCONST>::type, T>::value, int>::type = 0>
	SharedPtr( CreationPayload<NONCONST>&& payload )
		: PtrBase( rftl::move( payload ) )
	{
		//
	}

	// T -> DERIVED
	// Enable only if DERIVED is not a CV variation of T
	template<typename DERIVED, typename rftl::enable_if<rftl::is_same<typename rftl::remove_cv<DERIVED>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	SharedPtr( CreationPayload<DERIVED>&& payload )
		: PtrBase( rftl::move( payload ) )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
	}

	~SharedPtr()
	{
		PtrBase::DecreaseStrongCount();
	}

	SharedPtr& operator=( SharedPtr const& rhs )
	{
		SharedPtr temp( rhs );
		PtrBase::Swap( rftl::move( temp ) );
		return *this;
	}

	SharedPtr& operator=( SharedPtr&& rhs )
	{
		PtrBase::Swap( rftl::move( rhs ) );
		return *this;
	}

	T* Get() const
	{
		return (T*)( *this );
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
