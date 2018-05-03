#include "stdafx.h"
#include "TaskScheduler.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

void TaskScheduler::OnWorkComplete( TaskWorker * worker, Task * task, TaskState newState )
{
	// TODO: Deactivate task
	// TODO: Return/remove task to/from pool
	// TODO: Attempt dispatching
}



void TaskScheduler::OnTasksNewlyAvailable( TaskPool * pool )
{
	// TODO: Attempt dispatching
}

///////////////////////////////////////////////////////////////////////////////
}}
