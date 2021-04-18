#pragma once
#include "core/ptr/shared_ptr.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Shares weak reference to target, can create strong reference to target
template<typename T>
class WeakSharedPtr : public PtrBase<T>
{
	//
	// Types
public:
	using PtrBase = PtrBase<T>;


	//
	// Public methods
public:
	WeakSharedPtr()
		: PtrBase( nullptr, nullptr )
	{
		//
	}

	WeakSharedPtr( rftl::nullptr_t )
		: WeakSharedPtr()
	{
		//
	}

	WeakSharedPtr( WeakSharedPtr const& rhs )
		: WeakSharedPtr( rhs.GetTarget(), rhs.GetRef() )
	{
		//
	}

	WeakSharedPtr( T* target, PtrRef* ref )
		: PtrBase( target, ref )
	{
		PtrBase::IncreaseWeakCount();
	}

	WeakSharedPtr( WeakSharedPtr&& rhs )
		: PtrBase( rftl::move( rhs ) )
	{
		//
	}

	WeakSharedPtr( SharedPtr<T> const& rhs )
		: WeakSharedPtr( rhs.GetTarget(), rhs.GetRef() )
	{
		//
	}

	~WeakSharedPtr()
	{
		PtrBase::DecreaseWeakCount();
	}

	WeakSharedPtr& operator=( WeakSharedPtr const& rhs )
	{
		WeakSharedPtr temp( rhs );
		PtrBase::Swap( rftl::move( temp ) );
		return *this;
	}

	WeakSharedPtr& operator=( WeakSharedPtr&& rhs )
	{
		PtrBase::Swap( rftl::move( rhs ) );
		return *this;
	}

	WeakPtr<T> Weaken() const
	{
		return WeakPtr<T>( PtrBase::GetTarget(), PtrBase::GetRef() );
	}

	SharedPtr<T> Lock()
	{
		PtrBase::IncreaseStrongCount();
		PtrBase::SanitizeTarget();
		return PtrBase::CreateTransferPayload();
	}
};

///////////////////////////////////////////////////////////////////////////////
}
