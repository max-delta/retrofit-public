#include "stdafx.h"
#include "AsyncThread.h"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

AsyncThread::~AsyncThread()
{
	if( IsStarted() )
	{
		Stop();
	}
}



void AsyncThread::Init(
	PrepFunc&& prep,
	WorkFunc&& work,
	WorkConditionFunc&& workCheck,
	TerminateConditionFunc&& termCheck )
{
	if( IsStarted() )
	{
		RF_DBGFAIL_MSG( "Cannot init while already active" );
		return;
	}

	mPrep = rftl::move( prep );

	RF_ASSERT( work != nullptr );
	mWork = rftl::move( work );

	RF_ASSERT( workCheck != nullptr );
	mWorkCondition = rftl::move( workCheck );

	mTerminateCondition = rftl::move( termCheck );
}



bool AsyncThread::IsStarted() const
{
	return mThread.joinable();
}



void AsyncThread::Start()
{
	if( IsStarted() )
	{
		RF_DBGFAIL();
		return;
	}

	RF_ASSERT( mRunFlag.load( rftl::memory_order::memory_order_acquire ) == false );
	mRunFlag.store( true, rftl::memory_order::memory_order_release );
	mThread = rftl::thread( WorkThread, rftl::ref( *this ) );
	RF_ASSERT( IsStarted() );
}



void AsyncThread::Stop()
{
	if( IsStarted() == false )
	{
		RF_DBGFAIL();
		return;
	}

	RF_ASSERT( mRunFlag.load( rftl::memory_order::memory_order_acquire ) );
	mRunFlag.store( false, rftl::memory_order::memory_order_release );
	Wake();
	mThread.join();
	mThread = {};
	RF_ASSERT( IsStarted() == false );
}



void AsyncThread::Wake()
{
	mWakeSignal.Signal();
}



void AsyncThread::SetSafetyWakeupInterval( rftl::chrono::nanoseconds duration )
{
	mSafetyWakeupInterval.store(
		rftl::chrono::duration_cast<SafetyTime>( duration ).count(),
		rftl::memory_order::memory_order_release );
}

///////////////////////////////////////////////////////////////////////////////

void AsyncThread::WorkThread( AsyncThread& parent )
{
	static constexpr auto shouldForceTerminate = []( AsyncThread& parent ) -> bool //
	{
		return parent.mRunFlag.load( rftl::memory_order::memory_order_acquire ) == false;
	};

	// Prep
	if( parent.mPrep != nullptr )
	{
		parent.mPrep();
	}

	// Wakeup loop
	while( true )
	{
		// Batch loop
		while( true )
		{
			if( shouldForceTerminate( parent ) )
			{
				// Shutdown (forced)
				return;
			}

			if( parent.mTerminateCondition != nullptr && parent.mTerminateCondition() )
			{
				// Shutdown (graceful)
				return;
			}

			if( parent.mWorkCondition() == false )
			{
				// End batch
				break;
			}

			// Do work
			parent.mWork();
		}

		// Wait for activity
		parent.mWakeSignal.WaitFor( SafetyTime( parent.mSafetyWakeupInterval ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
