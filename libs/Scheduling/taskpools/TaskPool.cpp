#include "stdafx.h"
#include "TaskPool.h"

#include "Scheduling/TaskScheduler.h"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

void TaskPool::OnTasksNewlyAvailable( TaskScheduler * scheduler )
{
	scheduler->OnTasksNewlyAvailable( this );
}

///////////////////////////////////////////////////////////////////////////////
}}
