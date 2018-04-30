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

	// Add/remove task to/from the task pool, using unique identifers
	virtual TaskID AddTask( TaskPtr&& task ) = 0;
	virtual TaskPtr RemoveTask( TaskID taskID ) = 0;

	// Fetch new tasks, blocking them to prevent a subsequent fetch from seeing
	//  the same task
	// NOTE: Null task and invalid id if there is no unblocked task available
	virtual void FetchAndBlockNextUnblockedTask( Task*& task, TaskID& taskID ) = 0;

	// Unblock a previously blocked task
	// NOTE: Invalid to unblock an already unblocked task
	virtual void UnblockTask( TaskID taskID ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
