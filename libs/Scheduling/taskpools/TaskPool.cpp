#include "stdafx.h"
#include "TaskPool.h"

#include "Scheduling/TaskScheduler.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

void TaskPool::SetListener( TaskPoolListener && listener )
{
	rftl::lock_guard<rftl::mutex> lock( mListenerLock );
	mListener = rftl::move( listener );
}

///////////////////////////////////////////////////////////////////////////////

void TaskPool::SetScheduler( TaskScheduler * scheduler )
{
	mScheduler.store( scheduler, rftl::memory_order::memory_order_release );
}



void TaskPool::OnTasksNewlyAvailable()
{
	TaskScheduler* scheduler = mScheduler.load( rftl::memory_order::memory_order_acquire );
	if( scheduler )
	{
		scheduler->OnTasksNewlyAvailable( this );
	}
}



void TaskPool::OnTaskRemoved( TaskPtr && task, TaskID taskID, TaskState newState )
{
	rftl::lock_guard<rftl::mutex> lock( mListenerLock );
	if( mListener.mOnTaskRemoved != nullptr )
	{
		mListener.mOnTaskRemoved( rftl::move( task ), taskID, newState );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
