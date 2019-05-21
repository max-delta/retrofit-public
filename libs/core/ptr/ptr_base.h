#pragma once
#include "core/ptr/ptr_fwd.h"
#include "core/ptr/ptr_ref.h"
#include "core/ptr/creation_payload.h"
#include "core/ptr/ptr_traits.h"

#include "rftl/utility"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class PtrBase
{
	RF_NO_COPY( PtrBase );

	//
	// Friends
private:
	template<typename T>
	friend struct PtrTransformer;


	//
	// Public methods
public:
	// T -> T
	PtrBase( CreationPayload<T>&& payload )
		: mTarget( payload.mTarget )
		, mRef( payload.mRef )
	{
		payload.Clean();
	}

	// T -> T const
	// Enable only if T is a const version of NONCONST
	template<typename NONCONST, typename rftl::enable_if<rftl::is_same<typename rftl::add_const<NONCONST>::type, T>::value, int>::type = 0>
	PtrBase( CreationPayload<NONCONST>&& payload )
		: mTarget( payload.mTarget )
		, mRef( payload.mRef )
	{
		payload.Clean();
	}

	// T -> DERIVED
	// Enable only if DERIVED is not a CV variation of T
	template<typename DERIVED, typename rftl::enable_if<rftl::is_same<typename rftl::remove_cv<DERIVED>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	PtrBase( CreationPayload<DERIVED>&& payload )
		: mTarget( static_cast<T*>( payload.mTarget ) )
		, mRef( reinterpret_cast<PtrRef*>( payload.mRef ) )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		payload.Clean();
	}


	//
	// Protected methods
protected:
	explicit PtrBase( T* target, PtrRef* ref )
		: mTarget( target )
		, mRef( ref )
	{
		//
	}

	// T -> DERIVED
	// Enable only if DERIVED is not a CV variation of T
	template<typename DERIVED, typename rftl::enable_if<rftl::is_same<typename rftl::remove_cv<DERIVED>::type, typename rftl::remove_cv<T>::type>::value == false, int>::type = 0>
	explicit PtrBase( DERIVED* target, PtrRef* ref )
		: mTarget( static_cast<T*>( target ) )
		, mRef( ref )
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
	}

	explicit PtrBase( PtrBase&& rhs )
		: PtrBase( nullptr, nullptr )
	{
		Swap( rftl::move( rhs ) );
	}

	bool IsUniqueStrongOwner() const
	{
		if( mRef == nullptr )
		{
			return true;
		}
		return mRef->GetStrongCount() == 1;
	}

	void IncreaseStrongCount()
	{
		if( mRef != nullptr )
		{
			mRef->IncreaseStrongCount();
		}
	}

	void DecreaseStrongCount()
	{
		if( mRef != nullptr )
		{
			mRef->DecreaseStrongCount( mTarget );
		}
	}

	void IncreaseWeakCount()
	{
		if( mRef != nullptr )
		{
			mRef->IncreaseWeakCount();
		}
	}

	void DecreaseWeakCount()
	{
		if( mRef != nullptr )
		{
			mRef->DecreaseWeakCount();
		}
	}

	void Swap( PtrBase&& rhs )
	{
		rftl::swap( mRef, rhs.mRef );
		rftl::swap( mTarget, rhs.mTarget );
	}

	CreationPayload<T> CreateTransferPayload() const
	{
		return CreationPayload<T>( mTarget, mRef );
	}

	CreationPayload<T> CreateTransferPayloadAndWipeSelf()
	{
		T* const target = mTarget;
		PtrRef* const ref = mRef;
		mTarget = nullptr;
		mRef = nullptr;
		return CreationPayload<T>( target, ref );
	}

	void SanitizeTarget()
	{
		if( mTarget == nullptr )
		{
			return;
		}
		if( mRef->GetStrongCount() == 0 )
		{
			mTarget = nullptr;
		}
	}

	T* GetTarget() const
	{
		return mTarget;
	}

	T* GetTargetAsWeak() const
	{
		if( mTarget == nullptr )
		{
			return nullptr;
		}
		RF_ASSERT( mRef != nullptr );
		if( mRef->GetStrongCount() == 0 )
		{
			return nullptr;
		}
		return mTarget;
	}

	PtrRef* GetRef() const
	{
		return mRef;
	}


	//
	// Private data
private:
	T* mTarget;
	PtrRef* mRef;
};

///////////////////////////////////////////////////////////////////////////////
}
