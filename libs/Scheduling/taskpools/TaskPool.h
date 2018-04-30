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

	// Add a new task to the task pool, receiving a unique identifer for it
	virtual TaskID AddTask( TaskWeakPtr const& task ) = 0;

	// Attempt to abort a task, that either may not have run yet, or is
	//  incremental and is not currently running
	// NOTE: Already running or terminated tasks can not be aborted, and there
	//  is intentionally no method of stopping them once flighted, so if you
	//  need this functionality you will have to build it into your tasks
	//  themselves and establish a signalling mechanism where appopriate
	virtual void AttemptAbortTask( TaskID taskID ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
