#pragma once
#include "Task.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API NonIncrementalTask : public Task
{
	RF_NO_COPY( NonIncrementalTask );

	//
	// Public methods
public:
	NonIncrementalTask() = default;

	// Non-incremental tasks always terminate on step
	virtual TaskState Step() override final
	{
		Run();
		return TaskState::Terminate;
	}

	// Non-incremental tasks will never have this called on them
	virtual void Abort() override final
	{
		//
	}

	// Runs a task that must terminate before returning
	virtual void Run() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
