#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "core/macros.h"


namespace RF::scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API Task
{
	RF_NO_COPY( Task );

	//
	// Public methods
public:
	Task() = default;

	// NOTE: A task that is readied, but never run, may be cancelled and
	//  destroyed without having ever been stepped
	virtual ~Task() = default;

	// Attempts to clone a task
	// NOTE: Returns null if the task is not cloneable
	virtual TaskPtr Clone() const = 0;

	// Attempts to run a task incrementally, which may or may not result in
	//  the task terminating itself in the course of the step
	// NOTE: Valid return values are:
	//  * Ready - The task will need to be stepped again
	//  * Terminate - The task is complete and will need to be terminated
	virtual TaskState Step() = 0;

	// Aborts an incremental task that previously was stepped without the task
	//  self-terminating
	virtual void Abort() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
