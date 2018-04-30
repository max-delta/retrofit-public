#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "core/ptr/weak_ptr.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API TaskScheduler
{
	RF_NO_COPY( TaskScheduler );

	//
	// Public methods
public:
	// NOTE: Scheduler begins with dispatching disabled, and waits for it to
	//  become enabled by caller
	TaskScheduler() = default;

	// Register/unregister workers
	// NOTE: Effects not immediate, tasks may continue to be dispatched and
	//  processed by workers as though these calls were not made
	void RegisterWorker( WeakPtr<TaskWorker> const& worker );
	void UnregisterWorker( WeakPtr<TaskWorker> const& worker );

	// Add/abort tasks
	// NOTE: Already running or terminated tasks can not be aborted, and there
	//  is intentionally no method of stopping them once flighted, so if you
	//  need this functionality you will have to build it into your tasks
	//  themselves and establish a signalling mechanism where appopriate
	TaskID AddTask( TaskPtr&& task );
	void AttemptAbortTask( TaskID taskID );
	void AttemptAbortAllTasks();

	// Start/stop dispatching
	// NOTE: Effects not immediate, tasks may continue to be dispatched and
	//  processed as though these calls were not made
	// NOTE: Only applies to this scheduler, workers may be shared with other
	//  schedulers in some cases
	void StartDispatching();
	void StopDispatching( bool blockUntilWorkersIdle );

	// Performs an instant check that all tasks are complete, there is no
	//  further work to dispatch, and all workers registered with the scheduler
	//  are not performing tasks on its behalf
	// NOTE: Only applies to this scheduler, workers may be shared with other
	//  schedulers in some cases
	// WARNING: It is unsafe to perform any actions based on the result of this
	//  call unless the caller controls the tasks the scheduler works on and
	//  can prevent new tasks from being introduced
	bool AllTasksAreCurrentlyCompleted() const;


	//
	// Private methods
private:
	void OnWorkComplete( Task* task, TaskState newState );
};

///////////////////////////////////////////////////////////////////////////////
}}
