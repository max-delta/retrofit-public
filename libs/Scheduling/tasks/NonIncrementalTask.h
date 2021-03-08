#pragma once
#include "Task.h"


namespace RF::scheduling {
///////////////////////////////////////////////////////////////////////////////

class NonIncrementalTask : public Task
{
	RF_NO_COPY( NonIncrementalTask );

	//
	// Public methods
public:
	inline NonIncrementalTask() = default;
	virtual inline ~NonIncrementalTask() override = default;

	// Non-incremental tasks always terminate on step
	virtual inline TaskState Step() override final
	{
		Run();
		return TaskState::Terminate;
	}

	// Non-incremental tasks will never have this called on them
	virtual inline void Abort() override final
	{
		//
	}

	// Runs a task that must terminate before returning
	virtual void Run() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
