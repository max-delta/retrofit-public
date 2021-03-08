#include "stdafx.h"
#include "TaskWorker.h"

#include "Scheduling/TaskScheduler.h"


namespace RF::scheduling {
///////////////////////////////////////////////////////////////////////////////

void TaskWorker::OnWorkComplete( Task* task, TaskState newState, TaskScheduler* scheduler )
{
	scheduler->OnWorkComplete( this, task, newState );
}

///////////////////////////////////////////////////////////////////////////////
}
