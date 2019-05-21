#pragma once
#include "core/macros.h"

#include "rftl/cstdint"
#include "rftl/atomic"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

class PtrRef
{
	RF_NO_COPY( PtrRef );


	//
	// Types
public:
	typedef uint32_t CountType;
	typedef rftl::atomic<CountType> CountStorageType;
	typedef void ( *DeletionFunc )( void*, PtrRef*, void* );

	// NOTE: "Lock free" in this context applies to truly massive locks like
	//  mutexes and critical sections. CPU cache behavior and bus nuances are
	//  a seperate matter. For reference, the resulting CPU instructions of
	//  importance are listed near any atomic operations in this class.
	// NOTE: Seems like the x86 'lock' instruction on modern CPUs has a latency
	//  on par with the latency for an unshared cache-line hit on the worst
	//  local cache for that CPU before a read from RAM would be required. So
	//  it's still cheaper than basically all of your code, but you don't want
	//  to be changing refcounts in a tight loop. (Which you should have no
	//  reason to anyways...)
	static_assert( CountStorageType::is_always_lock_free, "Review atomic usage for this platform" );
	static_assert( sizeof( CountStorageType ) == sizeof( CountType ), "Review atomic usage for this platform" );


	//
	// Public methods
public:
	PtrRef( DeletionFunc deletionFunc, void* funcUserData )
		: mDeletionFunc( deletionFunc )
		, mFuncUserData( funcUserData )
		, mStrongCount( 1 )
		, mWeakCount( 1 )
	{
		RF_ASSERT( mDeletionFunc != nullptr );
	}

	void Delete( void* p ) const
	{
		RF_ASSERT( p != nullptr );
		RF_ASSERT( mDeletionFunc != nullptr );
		mDeletionFunc( p, nullptr, mFuncUserData );
	}

	void DeleteSelf()
	{
		RF_ASSERT( this != nullptr );
		RF_ASSERT( mDeletionFunc != nullptr );
		mDeletionFunc( nullptr, this, mFuncUserData );
	}

	CountType GetStrongCount() const
	{
		// x86 32/64:
		//  mov reg, [var]
		return mStrongCount.load( rftl::memory_order::memory_order_acquire );
	}

	CountType GetWeakCount() const
	{
		// x86 32/64:
		//  mov reg, [var]
		return mWeakCount.load( rftl::memory_order::memory_order_acquire );
	}

	void IncreaseStrongCount()
	{
		// NOTE: Another thread could theoretically drop the count to 0 between
		//  the load and add, so there's a minor spin that can occur if there's
		//  contention over the count, but that's unlikely unless someone's
		//  being a real idiot with their shared pointer usage
		while( true )
		{
			// x86 32/64:
			//  mov reg, [var]
			CountType const currentCount = mStrongCount.load( rftl::memory_order::memory_order_acquire );
			if( currentCount == 0 )
			{
				return;
			}

			// x86 32/64:
			//  lock cmpxchg [var], reg
			CountType expected = currentCount;
			bool const written = mStrongCount.compare_exchange_weak( expected, currentCount + 1, rftl::memory_order::memory_order_acq_rel );
			if( written )
			{
				return;
			}
		}
	}

	template<typename T>
	void DecreaseStrongCount( T*& target )
	{
		// x86 32/64:
		//  lock xadd [var], reg(-1)
		CountType const previousValue = mStrongCount.fetch_sub( 1, rftl::memory_order::memory_order_acq_rel );
		if( previousValue == 1 )
		{
			// NOTE: At this time, the strong count is 0, so any attempt to
			//  resolve a weak reference should result in a null reference
			//  while deletion is being performed, BUT... any previously
			//  resolved weak references that are now raw pointers will still
			//  be able to try and improperly access the memory
			// NOTE: Since T may have been const storage, we const_cast it away
			//  here, signifying that the constness guarantee is broken now
			using NonConstT = typename rftl::remove_const<T>::type;
			NonConstT* deletionTarget = const_cast<NonConstT*>( target );
			target = nullptr;
			Delete( deletionTarget );
			DecreaseWeakCount();
		}
	}

	void IncreaseWeakCount()
	{
		// x86 32/64:
		//  lock xadd [var], reg(1)
		mWeakCount.fetch_add( 1, rftl::memory_order::memory_order_release );
	}

	void DecreaseWeakCount()
	{
		// x86 32/64:
		//  lock xadd [var], reg(-1)
		CountType const previousValue = mWeakCount.fetch_sub( 1, rftl::memory_order::memory_order_acq_rel );
		if( previousValue == 1 )
		{
			DeleteSelf();
		}
	}


	//
	// Private data
private:
	DeletionFunc mDeletionFunc;
	void* mFuncUserData;
	CountStorageType mStrongCount;
	CountStorageType mWeakCount;
};

///////////////////////////////////////////////////////////////////////////////
}
