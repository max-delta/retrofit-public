#include "stdafx.h"
#include "TaskScheduler.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

WeakPtr<TaskWorker> TaskScheduler::RegisterWorker( UniquePtr<TaskWorker>&& worker )
{
	// TODO: Store worker
	// TODO: Attempt dispatching
	return WeakPtr<TaskWorker>();
}



void TaskScheduler::UnregisterWorker( WeakPtr<TaskWorker> const & worker )
{
	// TODO: If inactive, remove
	// TODO: If active, flag for removal
}



WeakPtr<TaskPool> TaskScheduler::RegisterPool( UniquePtr<TaskPool>&& pool, TaskPriority priority )
{
	// TODO: Store pool
	// TODO: Attempt dispatching
	return WeakPtr<TaskPool>();
}



void TaskScheduler::UnregisterPool( WeakPtr<TaskPool> const & pool )
{
	// TODO: Remove
}



void TaskScheduler::StartDispatching()
{
	RF_ASSERT( mDispatchingPermitted == false );
	mDispatchingPermitted = true;
	// TODO: Attempt dispatching
}



void TaskScheduler::StopDispatching( bool blockUntilWorkersIdle )
{
	RF_ASSERT( mDispatchingPermitted == true );
	mDispatchingPermitted = false;
	if( blockUntilWorkersIdle )
	{
		// TODO: Block until workers idle
	}
}



bool TaskScheduler::AllTasksAreCurrentlyCompleted() const
{
	RF_ASSERT_MSG(
		mDispatchingPermitted == false,
		"It is meaninglessly dangerous to check worker state when workers can"
		" still receive newly dispatched work" );
	// TODO: Check all workers
	// TODO: Check all pools (fetch and return === peek)
	return false;
}

///////////////////////////////////////////////////////////////////////////////

void TaskScheduler::OnWorkComplete( TaskWorker * worker, Task * task, TaskState newState )
{
	// TODO: Deactivate task
	// TODO: Return/remove task to/from pool
	// TODO: Attempt dispatching
}



void TaskScheduler::OnTasksNewlyAvailable( TaskPool * pool )
{
	// TODO: Attempt dispatching
}

///////////////////////////////////////////////////////////////////////////////

void TaskScheduler::AttemptDispatch()
{
	// TODO: Loop through workers
	// TODO: For each inactive worker, attempt dispatch to worker
}



void TaskScheduler::AttemptDispatch( TaskWorker* worker )
{
	// TODO: Loop through priority bins
	// TODO: For each priority bin, loop through each pool, starting with next
	//  round-robin selection
	// TODO: For each pool, attempt to fetch
	// TODO: On fetch success, push work to worker and return
}

///////////////////////////////////////////////////////////////////////////////
}}
