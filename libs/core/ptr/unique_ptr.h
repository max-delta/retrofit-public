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
	RF_NO_COPY(UniquePtr);


	//
	// Friends
private:
	template<typename PEER> friend class UniquePtr;


	//
	// Types
public:
	typedef PtrBase<T> PtrBase;


	//
	// Public methods
public:
	UniquePtr()
		: PtrBase(nullptr, nullptr)
	{
		//
	}

	UniquePtr( rftl::nullptr_t)
		: UniquePtr()
	{
		//
	}

	UniquePtr( UniquePtr && rhs )
		: PtrBase( rftl::move( rhs ) )
	{
		//
	}

	template<typename DERIVED>
	UniquePtr( UniquePtr<DERIVED> && rhs )
		: PtrBase( rftl::move( rhs.CreateTransferPayloadAndWipeSelf() ) )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		RF_ASSERT( rhs == nullptr );
	}

	UniquePtr( CreationPayload<T> && payload )
		: PtrBase( rftl::move( payload ) )
	{
		if( PtrBase::IsUniqueStrongOwner() == false )
		{
			RF_DBGFAIL_MSG( "UniquePtr is not unique after creation" );
			rftl::abort();
		}
	}

	template<typename DERIVED>
	UniquePtr( CreationPayload<DERIVED> && payload )
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

	UniquePtr & operator =(UniquePtr && rhs)
	{
		PtrBase::Swap( rftl::move(rhs) );
		return *this;
	}

	T * Get() const
	{
		return static_cast<T*>( *this );
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
