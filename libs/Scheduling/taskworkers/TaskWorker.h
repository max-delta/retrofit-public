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
	virtual void AddTask( Task* task ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}
