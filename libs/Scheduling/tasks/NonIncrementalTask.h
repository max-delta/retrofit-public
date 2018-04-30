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
	virtual State Step() override final
	{
		Run();
		return State::Terminate;
	}

	// Runs a task that must terminate before returning
	virtual void Run() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
