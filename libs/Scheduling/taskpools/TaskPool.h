#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"
#include "Scheduling/taskpools/TaskPoolListener.h"

#include "core/macros.h"

#include "rftl/mutex"


namespace RF::scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API TaskPool
{
	RF_NO_COPY( TaskPool );

	//
	// Friends
protected:
	friend class TaskScheduler;


	//
	// Public methods
public:
	TaskPool() = default;
	virtual ~TaskPool() = default;

	// Sets the listener for task pool events
	// NOTE: There is only one, intended to be set by the 'owner' of the pool,
	//  if such a concept applies to the instance
	void SetListener( TaskPoolListener&& listener );

	// Add/abort tasks
	// NOTE: Already running or terminated tasks can not be aborted, and there
	//  is intentionally no method of stopping them once flighted, so if you
	//  need this functionality you will have to build it into your tasks
	//  themselves and establish a signalling mechanism where appopriate
	// NOTE: Implementors can NOT remove a task on abortion if it is already
	//  flighted/blocked, since the memory is potentially in use and being run,
	//  but a flighted task may be returned and so an implementor should
	//  perform the abortion then to prevent it from being re-fetched
	virtual TaskID AddTask( TaskPtr&& task ) = 0;
	virtual void AttemptAbortTask( TaskID taskID ) = 0;
	virtual void AttemptAbortAllTasks() = 0;

	// Fetch new tasks, blocking them to prevent a subsequent fetch from seeing
	//  the same task
	// NOTE: Null task and invalid id if there is no unblocked task available
	virtual void FetchAndBlockNextUnblockedTask( Task*& task, TaskID& taskID ) = 0;

	// Unblock a previously blocked task that was fetched
	// NOTE: Invalid to unblock an already unblocked task
	// NOTE: The state is purely informative, the implementor can ignore it
	virtual void ReturnAndUnblockTask( TaskID taskID, TaskState newState ) = 0;

	// Remove a previously blocked task that was fetched
	// NOTE: The state is purely informative, the implementor can ignore it
	virtual void RemoveBlockedTask( TaskID taskID, TaskState newState ) = 0;


	//
	// Protected methods
protected:
	// Called by the scheduler when taking or releasing ownership
	void SetScheduler( TaskScheduler* scheduler );

	// Called when the last fetch found no unblocked work, but new activity
	//  unrelated to the scheduler has made tasks available, and the scheduler
	//  should be made aware in case it was idle
	// NOTE: Very likely to cause re-entrancy via fetch calls
	void OnTasksNewlyAvailable();

	// Called when a task is being removed
	void OnTaskRemoved( TaskPtr&& task, TaskID taskID, TaskState newState );


	//
	// Private data
private:
	rftl::atomic<TaskScheduler*> mScheduler = nullptr;
	TaskPoolListener mListener;
	rftl::mutex mListenerLock;
};

///////////////////////////////////////////////////////////////////////////////
}
