#pragma once
#include "project.h"

#include "core/ptr/ptr_fwd.h"

#include "rftl/cstdint"
#include "rftl/utility"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

enum class TaskState : uint8_t
{
	Invalid = 0,

	Ready = 1,
	Running = 2,
	Terminate = 3,

	MaxVal = 0x3u // NOTE: Usable as mask
};

enum class TaskPriority : uint8_t
{
	Invalid = 0,

	Idle = 4,
	Normal = 8,
	High = 13,
	RealTime = 24,

	MaxVal = 0x1fu // NOTE: Usable as mask
};

static constexpr uint8_t kTaskStateMask = static_cast<uint8_t>( TaskState::MaxVal );
static constexpr uint8_t kTaskPriorityMask = static_cast<uint8_t>( TaskPriority::MaxVal );

using TaskID = uint64_t;
constexpr TaskID kInvalidTaskID = 0;

class Task;
class NonIncrementalTask;
template<typename Functor>
class FunctorTask;

class TaskPool;
class TaskWorker;
class TaskScheduler;

using TaskPtr = UniquePtr<Task>;
using TaskWeakPtr = WeakPtr<Task>;
using TaskIDWeakPair = rftl::pair<TaskID, TaskWeakPtr>;

///////////////////////////////////////////////////////////////////////////////
}}
