#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "core/macros.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API TaskWorker
{
	RF_NO_COPY( TaskWorker );

	//
	// Public methods
public:
	TaskWorker() = default;
	virtual ~TaskWorker() = default;

	// Add a task to the worker
	// NOTE: Tasks will be executed in an arbitrary order, depending on the
	//  implementation details of a given worker
	virtual void AddTask( Task* task, TaskScheduler* scheduler ) = 0;


	//
	// Protected methods
protected:
	// Called when work is complete to notify the scheduler, so it can complete
	//  the task and possibly dispatch more work
	// NOTE: Very likely to cause re-entrancy via AddTask(...)
	void OnWorkComplete( Task* task, TaskState newState, TaskScheduler* scheduler );
};

///////////////////////////////////////////////////////////////////////////////
}}
