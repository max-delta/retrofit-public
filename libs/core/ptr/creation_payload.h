#pragma once
#include "core/macros.h"
#include "core/ptr/ptr_ref.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class CreationPayload
{
	RF_NO_COPY( CreationPayload );


	//
	// Friends
private:
	template<typename T> friend class PtrBase;
	template<typename T> friend struct PtrTransformer;


	//
	// Public methods
public:
	CreationPayload( T* target, PtrRef* ref )
		: mTarget( target )
		, mRef( ref )
	{
		//
	}

	CreationPayload( CreationPayload&& rhs )
		: mTarget( rftl::move( rhs.mTarget ) )
		, mRef( rftl::move( rhs.mRef ) )
	{
		rhs.Clean();
	}

	~CreationPayload()
	{
		RF_ASSERT( mTarget == nullptr );
		RF_ASSERT( mRef == nullptr );
		if( mRef != nullptr )
		{
			// Someone dropped the ball during transfer!
			mRef->Delete( mTarget );
			delete mRef;
		}
	}


	//
	// Private methods
private:
	void Clean()
	{
		mTarget = nullptr;
		mRef = nullptr;
	}


	//
	// Private data
private:
	T* mTarget;
	PtrRef* mRef;
};

///////////////////////////////////////////////////////////////////////////////
}
