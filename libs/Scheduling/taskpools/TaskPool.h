#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "core/macros.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API TaskPool
{
	RF_NO_COPY( TaskPool );

	//
	// Public methods
public:
	TaskPool() = default;
	virtual ~TaskPool() = default;

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
	virtual TaskPtr RemoveBlockedTask( TaskID taskID, TaskState newState ) = 0;


	//
	// Protected methods
protected:
	// Called when the last fetch found no unblocked work, but new activity
	//  unrelated to the scheduler has made tasks available, and the scheduler
	//  should be made aware in case it was idle
	// NOTE: Very likely to cause re-entrancy via fetch calls
	void OnTasksNewlyAvailable( TaskScheduler* scheduler );
};

///////////////////////////////////////////////////////////////////////////////
}}
