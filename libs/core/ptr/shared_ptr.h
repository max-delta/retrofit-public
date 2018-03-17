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
	// Friends
private:
	template<typename PEER> friend class SharedPtr;
	template<typename LIASON> friend class WeakSharedPtr;


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
		PtrBase::IncreaseStrongCount();
	}

	template<typename DERIVED>
	SharedPtr( SharedPtr<DERIVED> const & rhs )
		: PtrBase(rhs.GetTarget(), rhs.GetRef())
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		PtrBase::IncreaseStrongCount();
	}

	SharedPtr( SharedPtr && rhs )
		: PtrBase(std::move(rhs))
	{
		//
	}

	template<typename DERIVED>
	SharedPtr( SharedPtr<DERIVED> && rhs )
		: PtrBase(std::move(rhs.CreateTransferPayloadAndWipeSelf()))
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		RF_ASSERT( rhs == nullptr );
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
		PtrBase::DecreaseStrongCount();
	}

	SharedPtr & operator =(SharedPtr const & rhs)
	{
		SharedPtr temp(rhs);
		PtrBase::Swap( std::move(temp) );
		return *this;
	}

	SharedPtr & operator =(SharedPtr && rhs)
	{
		PtrBase::Swap( std::move(rhs) );
		return *this;
	}

	T * Get() const
	{
		return (T*)( *this );
	}

	operator T *() const
	{
		return PtrBase::GetTarget();
	}

	T* operator ->() const
	{
		return PtrBase::GetTarget();
	}

	operator WeakPtr<T>() const
	{
		return WeakPtr<T>( PtrBase::GetTarget(), PtrBase::GetRef() );
	}

	template<typename BASE>
	operator WeakPtr<BASE>() const
	{
		RF_PTR_ASSERT_CASTABLE( BASE, T );
		return WeakPtr<BASE>( PtrBase::GetTarget(), PtrBase::GetRef() );
	}
};

///////////////////////////////////////////////////////////////////////////////
}
