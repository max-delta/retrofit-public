#pragma once
#include "project.h"

#include "Scheduling/SchedulingFwd.h"

#include "rftl/functional"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

struct SCHEDULING_API TaskPoolListener
{
	using OnTaskRemoved = rftl::function<void( TaskPtr && task, TaskID id, TaskState newState )>;
	OnTaskRemoved mOnTaskRemoved = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}}
