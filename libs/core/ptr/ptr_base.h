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
	PtrBase( CreationPayload<T>&& payload )
		: mTarget( payload.mTarget )
		, mRef( payload.mRef )
	{
		payload.Clean();
	}

	template<typename DERIVED>
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

	template<typename DERIVED>
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
